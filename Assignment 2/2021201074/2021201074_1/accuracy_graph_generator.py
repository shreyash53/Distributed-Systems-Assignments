import subprocess
import matplotlib.pyplot as plt
import numpy as np

def run_command(command):
    process = subprocess.Popen(' '.join(command), shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    stdout, stderr = process.communicate()
    if stdout:
        print(stdout.decode())
    if stderr:
        print(stderr.decode())
    return stdout.decode()

x_list = [10, 100, 1000, 10000, 100000, 1000000]
y_list = []
for val in x_list:    
    op = run_command(['echo {0}'.format(val), '|', 'python3 mapper.py', '|', 'sort -k1,1', '|', 'python3 reducer.py'])
    # acc = 1 - abs(float(op) - np.e)/100
    acc = 1 - (abs(float(op)-np.e)/np.e)
    y_list.append(acc)

print(y_list)
plt.xlabel('No. of iterations ->')
plt.ylabel('accuracy ->')
plt.scatter(x_list, y_list)
plt.plot(x_list, y_list)
plt.savefig('accuracy_plot.png')