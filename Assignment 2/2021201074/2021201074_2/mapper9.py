#!/usr/bin/env python
import sys

m_r=2
m_c=3
n_r=3
n_c=2

i=0
m, n = map(int, sys.stdin.readline().strip().split())
for _ in range(m):
    el=list(map(int, sys.stdin.readline().strip().split()))
    for j in range(n):
        for k in range(m): 
            print("{0}\t{1}\t{2}\t{3}".format(i, k, j, el[j]))
    i+=1
        
n, p = map(int, sys.stdin.readline().strip().split())
for _ in range(n):
    el=list(map(int,sys.stdin.readline().strip().split()))
    for j in range(p):
        for k in range(n): 
            print("{0}\t{1}\t{2}\t{3}".format(k, j, i-m_r, el[j]))
    i=i+1