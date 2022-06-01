#!/usr/bin/python3

import sys


prev_item, name = None, None
users, ratings = [], []

for line in sys.stdin:
    key, value = line.strip().split('\t')
    item, tag = key.split(',')
    if item != prev_item:
        if prev_item != None:
            for user, rating in zip(users, ratings):
                print(f'{user}\t{name}@{rating}')
        prev_item = item
        users, ratings = [], []
    if tag == 'R':
        user, rating = value.split(',')
        users.append(user)
        ratings.append(rating)
    else:
        name = value

for user, rating in zip(users, ratings):
    print(f'{user}\t{name}@{rating}')
