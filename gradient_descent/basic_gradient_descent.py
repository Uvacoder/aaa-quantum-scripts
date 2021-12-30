import matplotlib.pyplot as plt
import numpy as np


def f(x, w, b):
    # Sigmoid Function
    f = 1 / (1 + np.exp(-(w * x + b)))
    return f


def mse(x, y, w, b):
    # Mean Squared Loss Function
    L = 0.0
    for i in range(x.shape[0]):
        L += 0.5 * (y[i] - f(x[i], w, b)) ** 2
    return L


def cross_entropy(x, y, w, b):
    # Cross Entropy Loss Function
    L = 0.0
    for i in range(x.shape[0]):
        L += -(y[i] * np.log(f(x[i], w, b)))
    return L


def grad_w_mse(x, y, w, b):
    fx = f(x, w, b)
    dw = (fx - y) * fx * (1 - fx) * x
    return dw


def grad_b_mse(x, y, w, b):
    fx = f(x, w, b)
    db = (fx - y) * fx * (1 - fx)
    return db


def grad_w_cross(x, y, w, b):
    fx = f(x, w, b)
    dw = (-y) * (1 - fx) * x
    return dw


def grad_b_cross(x, y, w, b):
    fx = f(x, w, b)
    db = (-y) * (1 - fx)
    return db


def GD(x, y, epochs, batch_size, loss, lr):
    w = np.random.randn()
    b = np.random.randn()
    l_list = []
    w_list = []
    b_list = []
    points = 0
    ep = [i for i in range(epochs + 1)]
    dw, db = 0, 0
    for i in range(epochs + 1):
        dw, db = 0, 0
        for j in range(x.shape[0]):
            if loss == "mse":
                dw += grad_w_mse(x[j], y[j], w, b)
                db += grad_b_mse(x[j], y[j], w, b)
            elif loss == "cross_entropy":
                dw += grad_w_cross(x[j], y[j], w, b)
                db += grad_b_cross(x[j], y[j], w, b)
            points += 1
            if points % batch_size == 0:
                w = w - lr * dw
                b = b - lr * db
                dw, db = 0, 0
        if loss == "mse":
            print("Loss after {}th epoch = {}\n".format(i, mse(x, y, w, b)[0]))
            l_list.append(mse(x, y, w, b)[0])
        elif loss == "cross_entropy":
            print(
                "Loss after {}th epoch = {}\n".format(i, cross_entropy(x, y, w, b)[0])
            )
            l_list.append(cross_entropy(x, y, w, b)[0])
        w_list.append(w[0])
        b_list.append(b[0])
    plt.xlabel("Epochs")
    plt.ylabel("Loss")
    plt.title(
        "Loss vs Epoch Curve\nAlgotithm :Mini Batch Gradient Decent\nBatch Size = {}\nLearning Rate = {}\nLoss Function = {}".format(
            batch_size, lr, loss
        )
    )
    plt.plot(ep, l_list)
    plt.show()
    return w_list, b_list
