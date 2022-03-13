import numpy as np
from oracles import *
import scipy
import timeit
from scipy.special import expit


class GDClassifier:
    """
    Реализация метода градиентного спуска для произвольного
    оракула, соответствующего спецификации оракулов из модуля oracles.py
    """

    def __init__(
        self, loss_function, step_alpha=1, step_beta=0,
        tolerance=1e-5, max_iter=1000, **kwargs
    ):
        """
        loss_function - строка, отвечающая за функцию потерь классификатора.
        Может принимать значения:
        - 'binary_logistic' - бинарная логистическая регрессия
        step_alpha - float, параметр выбора шага из текста задания
        step_beta- float, параметр выбора шага из текста задания
        tolerance - точность, по достижении которой, необходимо прекратить оптимизацию.
        Необходимо использовать критерий выхода по модулю разности соседних значений функции:
        если |f(x_{k+1}) - f(x_{k})| < tolerance: то выход
        max_iter - максимальное число итераций
        **kwargs - аргументы, необходимые для инициализации
        """
        self.loss_func = loss_function
        self.alpha = step_alpha
        self.beta = step_beta
        self.tolerance = tolerance
        self.max_iter = max_iter
        if self.loss_func == 'binary_logistic':
            self.bin_log = BinaryLogistic(**kwargs)

    def fit(self, X, y, w_0=None, trace=False):
        """
        Обучение метода по выборке X с ответами y
        X - scipy.sparse.csr_matrix или двумерный numpy.array
        y - одномерный numpy array
        w_0 - начальное приближение в методе
        trace - переменная типа bool
        Если trace = True, то метод должен вернуть словарь history, содержащий информацию
        о поведении метода. Длина словаря history = количество итераций + 1 (начальное приближение)
        history['time']: list of floats, содержит интервалы времени между двумя итерациями метода
        history['func']: list of floats, содержит значения функции на каждой итерации
        (0 для самой первой точки)
        """
        if w_0 is not None:
            self.w = w_0
        else:
            self.w = np.zeros(X.shape[1])
        log_loss_res_prev = self.get_objective(X, y)
        history = {'time': [0.0], 'func': [log_loss_res_prev], 'accuracy': [np.mean(y == self.predict(X))]}
        start_time = timeit.default_timer()
        for i in range(self.max_iter):
            w_new = self.w - self.alpha/((i + 1) ** self.beta) * self.get_gradient(X, y)
            log_loss_res = self.bin_log.func(X, y, w_new)
            if trace:
                history['func'].append(log_loss_res)
                history['time'].append(timeit.default_timer() - start_time)
                history['accuracy'].append(np.mean(y == self.predict(X)))
                start_time = timeit.default_timer()
            if abs(log_loss_res - log_loss_res_prev) < self.tolerance:
                self.w = w_new
                return history
            log_loss_res_prev = log_loss_res
            self.w = w_new
        if trace:
            return history

    def predict(self, X):
        """
        Получение меток ответов на выборке X
        X - scipy.sparse.csr_matrix или двумерный numpy.array
        return: одномерный numpy array с предсказаниями
        """
        return np.sign(X @ self.w)

    def predict_proba(self, X):
        """
        Получение вероятностей принадлежности X к классу k
        X - scipy.sparse.csr_matrix или двумерный numpy.array
        return: двумерной numpy array, [i, k] значение соответветствует вероятности
        принадлежности i-го объекта к классу k
        """
        proba_1 = expit(X @ self.w)
        proba_0 = 1 - proba_1
        return np.vstack((proba_0, proba_1)).T

    def get_objective(self, X, y):
        """
        Получение значения целевой функции на выборке X с ответами y
        X - scipy.sparse.csr_matrix или двумерный numpy.array
        y - одномерный numpy array
        return: float
        """
        return self.bin_log.func(X, y, self.w)

    def get_gradient(self, X, y):
        """
        Получение значения градиента функции на выборке X с ответами y
        X - scipy.sparse.csr_matrix или двумерный numpy.array
        y - одномерный numpy array
        return: numpy array, размерность зависит от задачи
        """
        return self.bin_log.grad(X, y, self.w)

    def get_weights(self):
        """
        Получение значения весов функционала
        """
        return self.w


class SGDClassifier(GDClassifier):
    """
    Реализация метода стохастического градиентного спуска для произвольного
    оракула, соответствующего спецификации оракулов из модуля oracles.py
    """

    def __init__(
        self, loss_function, batch_size=128, step_alpha=1, step_beta=0,
        tolerance=1e-5, max_iter=1000, random_seed=153, **kwargs
    ):
        """
        loss_function - строка, отвечающая за функцию потерь классификатора.
        Может принимать значения:
        - 'binary_logistic' - бинарная логистическая регрессия
        batch_size - размер подвыборки, по которой считается градиент
        step_alpha - float, параметр выбора шага из текста задания
        step_beta- float, параметр выбора шага из текста задания
        tolerance - точность, по достижении которой, необходимо прекратить оптимизацию
        Необходимо использовать критерий выхода по модулю разности соседних значений функции:
        если |f(x_{k+1}) - f(x_{k})| < tolerance: то выход
        max_iter - максимальное число итераций (эпох)
        random_seed - в начале метода fit необходимо вызвать np.random.seed(random_seed).
        Этот параметр нужен для воспроизводимости результатов на разных машинах.
        **kwargs - аргументы, необходимые для инициализации
        """
        self.loss_func = loss_function
        self.batch_size = batch_size
        self.alpha = step_alpha
        self.beta = step_beta
        self.tolerance = tolerance
        self.max_iter = max_iter
        self.random_seed = random_seed
        if self.loss_func == 'binary_logistic':
            self.bin_log = BinaryLogistic(**kwargs)

    def fit(self, X, y, w_0=None, trace=False, log_freq=1):
        """
        Обучение метода по выборке X с ответами y
        X - scipy.sparse.csr_matrix или двумерный numpy.array
        y - одномерный numpy array
        w_0 - начальное приближение в методе
        Если trace = True, то метод должен вернуть словарь history, содержащий информацию
        о поведении метода. Если обновлять history после каждой итерации, метод перестанет
        превосходить в скорости метод GD. Поэтому, необходимо обновлять историю метода лишь
        после некоторого числа обработанных объектов в зависимости от приближённого номера эпохи.
        Приближённый номер эпохи:
            {количество объектов, обработанных методом SGD} / {количество объектов в выборке}
        log_freq - float от 0 до 1, параметр, отвечающий за частоту обновления.
        Обновление должно проиходить каждый раз, когда разница между двумя значениями приближённого номера эпохи
        будет превосходить log_freq.
        history['epoch_num']: list of floats, в каждом элементе списка будет записан приближённый номер эпохи:
        history['time']: list of floats, содержит интервалы времени между двумя соседними замерами
        history['func']: list of floats, содержит значения функции после текущего приближённого номера эпохи
        history['weights_diff']: list of floats, содержит квадрат нормы разности векторов весов с соседних замеров
        (0 для самой первой точки)
        """
        np.random.seed(self.random_seed)
        if w_0 is not None:
            self.w = w_0
        else:
            self.w = np.zeros(X.shape[1])
        if trace:
            history = {'time': [], 'func': [], 'weights_diff': [], 'epoch_num': [], 'accuracy': []}
        if isinstance(X, scipy.sparse.coo.coo_matrix):
            X = X.tocsr()
        num_processed, epoch_prev, i, w_prev = 0, 0, 0, self.w
        start_time = timeit.default_timer()
        log_loss_res_prev = self.bin_log.func(X, y, self.w)
        while i < self.max_iter:
            shuffle = np.random.permutation(X.shape[0])
            X_new = X[shuffle]
            y_new = y[shuffle]
            for j in range(X.shape[0] // self.batch_size + 1):
                i += 1
                if i > self.max_iter:
                    break
                X_batch = X_new[j*self.batch_size:(j+1)*self.batch_size]
                y_batch = y_new[j*self.batch_size:(j+1)*self.batch_size]
                if X_batch.shape[0] == 0:
                    continue
                eps = self.bin_log.func(X_batch, y_batch, self.w)
                self.w = self.w - self.alpha/(i**self.beta) * self.bin_log.grad(X_batch, y_batch, self.w)
                num_processed += X_batch.shape[0]
                log_loss_res = self.bin_log.func(X, y, self.w)
                if num_processed / X.shape[0] - epoch_prev > log_freq:
                    epoch_prev = num_processed / X.shape[0]
                    if trace:
                        history['func'].append(log_loss_res)
                        history['time'].append(timeit.default_timer() - start_time)
                        history['weights_diff'].append((np.linalg.norm(self.w - w_prev)**2))
                        history['epoch_num'].append(epoch_prev)
                        start_time, w_prev = timeit.default_timer(), self.w
                        history['accuracy'].append(np.mean(y == self.predict(X)))
                if abs(log_loss_res - log_loss_res_prev) < self.tolerance:
                    if trace:
                        history['func'].append(log_loss_res)
                        history['time'].append(timeit.default_timer() - start_time)
                        history['weights_diff'].append((np.linalg.norm(self.w - w_prev)**2))
                        history['epoch_num'].append(num_processed / X.shape[0])
                        history['accuracy'].append(np.mean(y == self.predict(X)))
                        return history
                    return
                log_loss_res_prev = log_loss_res
        if trace:
            return history
