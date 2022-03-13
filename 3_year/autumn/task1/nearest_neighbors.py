import numpy as np
from sklearn.neighbors import NearestNeighbors
from distances import *


class KNNClassifier():
    def __init__(self, k, strategy, metric, weights=False, test_block_size=100):
        self.k = k
        self.strategy = strategy
        self.metric = metric
        self.weights = weights
        self.size = test_block_size
        self.X_train = None
        self.y_train = None
        if strategy != 'my_own' and strategy != 'brute' and strategy != 'kd_tree' and strategy != 'ball_tree':
            raise TypeError("Nor right arguments for 'strategy'")
        if metric != 'euclidean' and metric != 'cosine':
            raise TypeError("Nor right arguments for 'metric'")
        if strategy == 'my_own':
            self.strat_class = None
        elif self.metric == 'cosine' and (self.strategy == 'kd_tree' or self.strategy == 'ball_tree'):
            raise TypeError("Nor right metric for strategy")
        else:
            self.strat_class = NearestNeighbors(n_neighbors=self.k, algorithm=strategy, metric=metric)

    def find_kneighbors(self, X, return_distance=True):
        if self.X_train is None:
            raise TypeError("This KNNClassifier instance is not fitted yet")
        if self.strategy == 'my_own':
            if self.metric == 'euclidean':
                Distances = euclidean_distance(X, self.X_train)
            else:
                Distances = cosine_distance(X, self.X_train)
            Idx = np.argsort(Distances, axis=1)[..., :self.k]
            Distances = np.sort(Distances, axis=1)[..., :self.k]
            if return_distance is True:
                return (Distances, Idx)
            else:
                return Idx
        else:
            return self.strat_class.kneighbors(X, self.k, return_distance=return_distance)

    def fit(self, X, y):
        self.X_train = np.copy(X)
        self.y_train = np.copy(y)
        if self.strategy != 'my_own':
            self.strat_class.fit(self.X_train)

    def predict(self, X):
        y_pred = np.array([])
        eps = 1e-5
        if X.shape[0] % self.size == 0:
            size = X.shape[0]//self.size
        else:
            size = X.shape[0]//self.size + 1
        for i in range(size):
            if self.weights:
                weights, neighbors = self.find_kneighbors(X[i*self.size:(i+1)*self.size], return_distance=True)
                weights = 1/(weights + eps)
                classes = self.y_train[neighbors]
                if self.k == 1:
                    y_pred = np.append(y_pred, classes)
                else:
                    dist = np.zeros(len(np.unique(self.y_train)))
                    for j in range(len(classes)):
                        for cl in range(len(np.unique(self.y_train))):
                            dist[cl] = weights[j][classes[j] == cl].sum()
                        idx = np.argmax(weights, axis=1)
                        y_pred = np.append(y_pred, np.argmax(dist))
                        dist[:] = 0
            else:
                neighbors = self.find_kneighbors(X[i*self.size:(i+1)*self.size], return_distance=False)
                for n in neighbors:
                    classes = self.y_train[n]
                    if self.k == 1:
                        y_pred = np.append(y_pred, classes)
                    else:
                        classes_count = np.bincount(classes)
                        y_pred = np.append(y_pred, np.argmax(classes_count))
        return y_pred
