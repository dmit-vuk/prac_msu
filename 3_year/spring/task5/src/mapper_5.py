#!/usr/bin/python3

import sys


for line in sys.stdin:
    line = line.strip()
    if len(line.split('\t')) > 1:
        print(line)
    else:
        try:
            inpt = line.split(',')
            print(f'{inpt[0]},{inpt[1]}\t{inpt[2]}')
        except:
            pass
