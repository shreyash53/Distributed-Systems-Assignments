#!/usr/bin/env python


import sys


def get_input(line):
    return line.strip().split()

# Read the first line of input containing the dimensions of A and B

A = []
B = []
for line in sys.stdin:
    m, n = map(int, get_input(line))
    break
# For each input line, split it into its row and column index and value
c = 0
for line in sys.stdin:
    vals = list(map(int, get_input(line)))
    A.append(vals)
    c += 1
    if(c == m):
        break

# n2, p = map(int, get_input())
for line in sys.stdin:
    n2, p = map(int, get_input(line))
    break

c=0
for line in sys.stdin:
        # Value belongs to matrix B
    vals = list(map(int, get_input(line)))
    B.append(vals)
    c += 1
    if(c == n2):
        break


for i in range(m):
    for j in range(n):
        for k in range(p):
            print("{0},{1},{2}\t{3}".format(i, k, j, A[i][j]))

for j in range(p):
    for i in range(n2):
        for k in range(m):
            print("{0},{1},{2}\t{3}".format(k, j, i, B[i][j]))

