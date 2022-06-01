#!/usr/bin/python3

import sys
import numpy as np

line = sys.stdin.readline().rstrip('\n')
prev_key, value = line.split('\t')
r_i, r_j = value.split(',')
ratings_i = np.array(float(r_i))
ratings_j = np.array(float(r_j))

for line in sys.stdin:
    line = line.strip()
    key, value = line.split('\t')
    r_i, r_j = value.split(',')
    if prev_key != key:
        numenator = np.dot(ratings_i, ratings_j)
        denominator = np.linalg.norm(ratings_i) * np.linalg.norm(ratings_j)
        sim = numenator / (denominator + 1e-9)
        if sim < 0:
            sim = 0
        item_i, item_j = prev_key.split(',')
        print(f"{item_i},{item_j}\t{sim:.{3}f}")
        prev_key = key
        ratings_i = np.array(float(r_i))
        ratings_j = np.array(float(r_j))
    else:
        ratings_i = np.append(ratings_i, float(r_i))
        ratings_j = np.append(ratings_j, float(r_j))

numenator = np.dot(ratings_i, ratings_j)
denominator = np.linalg.norm(ratings_i) * np.linalg.norm(ratings_j)
sim = numenator / (denominator + 1e-9)
if sim < 0:
    sim = 0
item_i, item_j = prev_key.split(',')
print(f"{item_i},{item_j}\t{sim:.{3}f}")