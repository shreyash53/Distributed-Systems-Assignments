#!/usr/bin/python

import sys

# Reducer function
def reducer():
    n_counts = {}
    for line in sys.stdin:
        n, count = line.strip().split("\t")
        n = int(n)
        count = int(count)
        if n in n_counts:
            n_counts[n] += count
        else:
            n_counts[n] = count
    total = sum(n_counts.values())
    expected_value = sum([n * count / total for n, count in n_counts.items()])
    print(expected_value)

reducer()
