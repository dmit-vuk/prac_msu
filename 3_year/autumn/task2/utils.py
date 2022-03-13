#!/usr/bin/env python
# coding: utf-8

# In[1]:


import numpy as np


# In[2]:


def grad_finite_diff(function, w, eps=1e-8):
    """
    Возвращает численное значение градиента, подсчитанное по следующией формуле:
        result_i := (f(w + eps * e_i) - f(w)) / eps,
        где e_i - следующий вектор:
        e_i = (0, 0, ..., 0, 1, 0, ..., 0)
                          >> i <<
    """
    res, e = np.zeros(len(w)), np.zeros(len(w))
    for i in range(len(w)):
        e[i] = 1
        res[i] = (function(w + eps * e) - function(w)) / eps
        e[i] = 0
    return res

