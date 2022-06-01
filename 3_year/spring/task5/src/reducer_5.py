#!/usr/bin/python3

import sys
import numpy as np

line = sys.stdin.readline().rstrip('\n')
key, rating = line.split('\t')
prev_user, item = key.split(',')
items = np.array([item])
ratings = np.array([float(rating)])

for line in sys.stdin:
    line = line.strip()
    key, rating = line.split('\t')
    user, item = key.split(',')
    if user != prev_user:
        idc = np.argsort(ratings)[::-1][:100]
        items = items[idc]
        ratings = ratings[idc]
        for item_id, rat in zip(items, ratings):
            print(f'{item_id}\t{prev_user},{rat:.{3}f}')
        prev_user = user
        items = np.array([item])
        ratings = np.array([float(rating)])
    if len(items) != 0 and items[-1] == item:
        items = items[:-1]
        ratings = ratings[:-1]
    else:
        items = np.append(items, item)
        ratings = np.append(ratings, float(rating))

idc = np.argsort(ratings)[::-1][:100]
items = items[idc]
ratings = ratings[idc]
for i, item_id in enumerate(items):
    print(f'{item_id}\t{prev_user},{rat:.{3}f}')
