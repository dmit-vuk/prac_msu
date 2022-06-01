#!/usr/bin/python3

import sys
import numpy as np

line = sys.stdin.readline().rstrip('\n')
user, values = line.split('\t')
item, rating = values.split(',')
item_ids = np.array(item)
ratings =  np.array(float(rating))

for line in sys.stdin:
    line = line.strip()
    key, values = line.split('\t')
    if key == user:
        item, rating = values.split(',')
        item_ids = np.append(item_ids, item)
        ratings = np.append(ratings, float(rating))
    else:
        ratings = ratings - ratings.mean()
        for i in range(len(item_ids)):
            for j in range(i + 1, len(item_ids)):
                print(f'{item_ids[i]},{item_ids[j]}\t{ratings[i]:.{3}f},{ratings[j]:.{3}f}')
        user = key
        item, rating = values.split(',')
        item_ids = np.array(item)
        ratings =  np.array(float(rating))

ratings = ratings - ratings.mean()
for i in range(len(item_ids)):
    for j in range(i + 1, len(item_ids)):
        print(f'{item_ids[i]},{item_ids[j]}\t{ratings[i]:.{3}f},{ratings[j]:.{3}f}')
