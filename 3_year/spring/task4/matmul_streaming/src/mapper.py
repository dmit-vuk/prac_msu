#!/usr/bin/python3

import os
import sys
import numpy as np

matrix = os.environ["mapreduce_map_input_file"]
N = int(os.environ["N"])
K = int(os.environ["K"])
# input comes from STDIN (standard input)

for line in sys.stdin:
    line = line.strip()
    row = np.array(line.split()).astype(float)
    
    if(matrix[-5:] == "A.txt"):
        for i in range(len(row[1:])):
            for j in range(K):
                print('{},{}\t{},{},{}'.format(int(row[0]),j,'A',i,row[i+1]))
    else:
        for i in range(len(row[1:])):
            for j in range(N):
                print('{},{}\t{},{},{}'.format(j, i, 'B', int(row[0]), row[i+1]))