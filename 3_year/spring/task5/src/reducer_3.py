#!/usr/bin/python3

import sys

line = sys.stdin.readline().rstrip('\n')
prev_key, value = line.split('\t')
flag, item_1, item_2 = value.split(',')
ratigs, sims = [], []
if flag == 'R':
    ratigs.append((item_1, item_2))
else:
    sims.append((item_1, item_2))

for line in sys.stdin:
    line = line.strip()
    key, value = line.split('\t')
    flag, item_1, item_2 = value.split(',')
    if key != prev_key:
        for user, rating in ratigs:
            for item, sim in sims:
                print(f'{user},{item}\t{rating},{sim}')
        prev_key = key
        ratigs, sims = [], []
    if flag == 'R':
        ratigs.append((item_1, item_2))
    else:
        sims.append((item_1, item_2))

for user, rating in ratigs:
    for item, sim in sims:
        print(f'{user},{item}\t{rating},{sim}')