 #!/usr/bin/env python

import sys


# def get_input():
#     return sys.stdin.readline().strip().split()

# # Read the first line of input containing the dimensions of A and B
# m, n = map(int, get_input())
# A = []
# # For each input line, split it into its row and column index and value
# c = 0
# for _ in range(m):
#     vals = list(map(int, get_input()))
#     A.append(vals)

# B = []
# n2, p = map(int, get_input())
# for _ in range(n2):
#         # Value belongs to matrix B
#     vals = list(map(int, get_input()))
#     B.append(vals)


# for i in range(m):
#     for j in range(n):
#         for k in range(p):
#             print("{0},{1},{2}\t{3}".format(i, k, j, A[i][j]))

# for j in range(p):
#     for i in range(n2):
#         for k in range(m):
#             print("{0},{1},{2}\t{3}".format(k, j, i, B[i][j]))

for line in sys.stdin:
    print(line,end='')