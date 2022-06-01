#!/usr/bin/python3

import os
import sys
import numpy as np

M = int(os.environ["M"])
A = np.zeros(M)
B = np.zeros(M)

key = None
current_key = None

for line in sys.stdin:
    line = line.strip()
    key, value = line.split('\t')
    values = value.split(',')
    

    # this IF-switch only works because Hadoop sorts map output
    # by key (here: word) before it is passed to the reducer
    if current_key == key:
        if values[0] == 'A':
            A[int(values[1])] = float(values[2])
        else:
            B[int(values[1])] = float(values[2])
    else:
        if current_key:
            # write result to STDOUT
            print(f'{current_key}\t{(A*B).sum():.{7}f}')
        current_key = key
        if values[0] == 'A':
            A[int(values[1])] = float(values[2])
        else:
            B[int(values[1])] = float(values[2])

# do not forget to output the last word if needed!
if current_key == key:
    if values[0] == 'A':
        A[int(values[1])] = float(values[2])
    else:
        B[int(values[1])] = float(values[2])
    print(f'{current_key}\t{(A*B).sum():.{7}f}')