import sys
import csv

sys.stdin = open('./data/input/ratings.csv')
sys.stdout = open('data/input/ratings_preproc.txt', 'w+')
sys.stdin.readline()

for line in sys.stdin:
    try:
        line = line.strip()
        inpt = line.split(',')
        print(f'{inpt[0]},{inpt[1]},{inpt[2]}')
    except:
        pass

sys.stdin = open('./data/input/movies.csv')
sys.stdout = open('data/input/movies_preproc.txt', 'w+')
sys.stdin.readline()

for line in sys.stdin:
    try:
        line = line.strip()
        line = [line]
        for item in csv.reader(line):
            print(f'{item[0]}@{item[1]}')
    except:
        pass