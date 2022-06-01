#!/usr/bin/python3

import sys
import numpy as np


prev_user = None
names, ratings = [], []

for line in sys.stdin:
    user, value = line.strip().split('\t')
    name, rating = value.split('@')
    if user != prev_user:
        if prev_user != None:
            print(f'{prev_user}', end='@')
            names = np.array(names)
            names = names[np.argsort(ratings)]
            ratings = np.sort(ratings)
            for i in range(len(ratings) - 1, -1, -1):
                print(f'{ratings[i]}%{names[i]}', end='')
                if i > 0:
                    print('@', end='')
            print('')
        prev_user = user
        names, ratings = [], []
    names.append(name)
    ratings.append(float(rating))

print(f'{prev_user}', end='@')
names = np.array(names)
names = names[np.argsort(ratings)]
ratings = np.sort(ratings)
for i in range(len(ratings) - 1, -1, -1):
    print(f'{ratings[i]}#%{names[i]}', end='')
    if i > 0:
        print('@', end='')
print('')
