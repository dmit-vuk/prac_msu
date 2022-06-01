#!/usr/bin/python3

import sys

for line in sys.stdin:
    try:
        line = line.strip()
        inpt = line.split(',')
        print(f'{inpt[0]}\t{inpt[1]},{inpt[2]}')
    except:
        pass
