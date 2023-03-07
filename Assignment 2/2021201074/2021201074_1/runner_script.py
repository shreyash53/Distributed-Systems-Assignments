import sys
import os
import subprocess

def get_input(line):
    return line.strip().split()

jar_path = '/opt/hadoop-3.2.4/share/hadoop/tools/lib/hadoop-streaming-3.2.4.jar'
# jar_path = sys.argv[1]
inp_file_path = sys.argv[2]
inp_dir_path = sys.argv[3]
out_dir_path = sys.argv[4]
mapper_path = sys.argv[5]
reducer_path = sys.argv[6]


def run_command(command):
    process = subprocess.Popen(' '.join(command), shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    stdout, stderr = process.communicate()
    if stdout:
        print(stdout.decode())
    if stderr:
        print(stderr.decode())

#copy files in docker container
run_command(['sudo', 'docker', 'cp', "'" + inp_file_path + "'", 'namenode:/input.txt'])
run_command(['sudo', 'docker', 'cp', "'" + os.path.join(reducer_path, 'reducer.py') + "'", 'namenode:/reducer.py'])
run_command(['sudo', 'docker', 'cp', "'" + os.path.join(mapper_path, 'mapper.py') + "'", 'namenode:/mapper.py'])

# copy mapper_input to input directory
run_command(['sudo', 'docker', 'exec', 'namenode', 'hdfs', 'dfs', '-copyFromLocal', '-f', 'input.txt', inp_dir_path])

# # remove temporary local files
# run_command(['rm', 'tmp_input'])

# comment before submission
run_command(['sudo', 'docker', 'exec', 'namenode', 'hdfs', 'dfs', '-rm', '-r', out_dir_path])

cmd = ['sudo', 'docker', 'exec', 'namenode',
       "hadoop", "jar", jar_path,
       "-input", os.path.join(inp_dir_path, 'input.txt'),
       "-output", out_dir_path,
       "-mapper", "'python3 mapper.py'",
       "-reducer", "'python3 reducer.py'",
       "-file", "./reducer.py",
       "-file", "./mapper.py",
       ]

# print(' '.join(cmd))
run_command(cmd)

run_command(['sudo', 'docker', 'exec', 'namenode', 'hdfs', 'dfs', '-cat', os.path.join(out_dir_path, '*')])
# print('completed')