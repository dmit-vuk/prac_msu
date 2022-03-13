def euclidean_distance(X, Y):
    import numpy as np
    return np.sqrt((X*X).sum(axis=1).reshape(-1, 1) + (Y*Y).sum(axis=1) - 2*X@Y.T)


def cosine_distance(X, Y):
    import numpy as np
    X_norm = np.sqrt((X*X).sum(axis=1).reshape(-1, 1))
    Y_norm = np.sqrt((Y*Y).sum(axis=1))
    X_norm[X_norm == 0] = 0.001
    Y_norm[Y_norm == 0] = 0.001
    return 1 - ((X@Y.T) / (X_norm * Y_norm))
