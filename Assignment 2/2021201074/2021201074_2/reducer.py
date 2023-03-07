#!/usr/bin/env python

import sys

ans = 0
val = 0
flag = False
last_index_i = 0
last_index_j = 0
last_index_k = -1

for line in sys.stdin:
    cell = line.strip().split('\t')
    index = list(map(int, cell[0].split(',')))
    tmp = int(cell[1])

    if last_index_j != index[1]:
        ans += val
        print(ans, end=" ")
        val = 0
        ans = 0
        last_index_j = index[1]
    
    if last_index_i != index[0]:
        print()
        last_index_i = index[0]

    if last_index_k == index[2]:
        val *= tmp
    else:
        ans += val
        val = 0
        val += tmp
        last_index_k = index[2]

if(val != 0):
    print(val)
else:
    print(ans)