import numpy as np
from distances import *
from nearest_neighbors import *


def kfold(n, n_folds):
    folds = []
    num_test = n // n_folds
    dop_test = n % n_folds
    last_test_idx = -1
    all_idx = np.arange(n)
    for i in range(n_folds):
        if dop_test > 0:
            test_idx = np.array(range(last_test_idx + 1, last_test_idx + num_test + 2))
            train_idx = np.delete(all_idx, test_idx)
            last_test_idx = last_test_idx + num_test + 1
            dop_test -= 1
        else:
            test_idx = np.array(range(last_test_idx + 1, last_test_idx + num_test + 1))
            train_idx = np.delete(all_idx, test_idx)
            last_test_idx = last_test_idx + num_test
        tup = (train_idx.astype('int'), test_idx.astype('int'))
        folds.append(tup)
    return folds


def knn_cross_val_score(X, y, k_list, score='accuracy', cv=None, **kwargs):
    if cv is None:
        cv = kfold(X.shape[0], 3)
    res = {}
    i = -1
    for c in cv:
        i += 1
        cl = KNNClassifier(k_list[-1], **kwargs)
        train_folds, test_folds = c[0], c[1]
        X_train, y_train = X[train_folds], y[train_folds]
        X_test, y_test = X[test_folds], y[test_folds]
        cl.fit(X_train, y_train)
        dist = np.zeros((len(y_test), len(np.unique(y))))
        if cl.weights:
            eps = 1e-5
            w, neigh = cl.find_kneighbors(X_test, return_distance=True)
            w = 1/(w + eps)
        else:
            neigh = cl.find_kneighbors(X_test, return_distance=False)
        classes = y_train[neigh]
        len_test = len(y_test)
        for k in range(1, k_list[-1] + 1):
            y_pred = np.zeros(neigh.shape[0])
            if cl.weights:
                for j in range(classes.shape[0]):
                    dist[j][classes[j, k-1]] += w[j, k-1]
                    y_pred[j] = np.argmax(dist[j])
            else:
                for j in range(classes.shape[0]):
                    dist[j][classes[j, k-1]] += 1
                    y_pred[j] = np.argmax(dist[j])
            if k in k_list:
                if k not in res.keys():
                    res[k] = np.zeros(len(cv))
                res[k][i] = np.count_nonzero(y_pred == y_test) / len_test
    return res
