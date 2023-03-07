#!/usr/bin/python

import sys
import random

# Mapper function
def mapper(num_samples):
    for i in range(num_samples):
        n = 0
        total = 0
        while total < 1:
            total += random.uniform(0, 1)
            n += 1
        yield n, 1
        
        
for a in sys.stdin:
    try:
        num_samples = int(a)
        for n, count in mapper(num_samples):
            print("{0}\t{1}".format(n, count))
    except:
        pass