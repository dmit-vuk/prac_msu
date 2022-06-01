#!/usr/bin/python3

import sys

for line in sys.stdin:
    line = line.strip()
    if len(line.split('\t')) > 1:
        key, sim = line.split('\t')
        item_i, item_j = key.strip().split(',')
        print(f'{item_i}\tS,{item_j},{sim}')
        print(f'{item_j}\tS,{item_i},{sim}')
    else:
        try:
            inpt = line.split(',')
            print(f'{inpt[1]}\tR,{inpt[0]},{inpt[2]}')
        except:
            pass
