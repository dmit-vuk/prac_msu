#!/usr/bin/python3

import sys
import numpy as np

line = sys.stdin.readline().rstrip('\n')
prev_key, value = line.split('\t')
rating, sim = value.split(',')
ratings = np.array(float(rating))
sims = np.array(float(sim))

for line in sys.stdin:
    line = line.strip()
    key, value = line.split('\t')
    rating, sim = value.split(',')
    if key != prev_key:
        rating_pred = (ratings * sims).sum() / (sims.sum() + 1e-9)
        user, item = prev_key.split(',')
        print(f'{user},{item}\t{rating_pred:.{3}f}')
        ratings = np.array(float(rating))
        sims = np.array(float(sim))
        prev_key = key
    else:
        ratings = np.append(ratings, float(rating))
        sims = np.append(sims, float(sim))

rating_pred = (ratings * sims).sum() / (sims.sum() + 1e-9)
user, item = prev_key.split(',')
print(f'{user},{item}\t{rating_pred:.{3}f}')