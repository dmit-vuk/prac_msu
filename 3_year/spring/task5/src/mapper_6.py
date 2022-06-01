#!/usr/bin/python3

import sys


for line in sys.stdin:
    line = line.strip()
    if len(line.split('@')) == 1:
        key, value = line.split('\t')
        print(f'{key},R\t{value}')
    else:
        try:
            key, value = line.split('@')
            print(f'{key},N\t{value}')
        except:
            pass
