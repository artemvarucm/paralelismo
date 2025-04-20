import numpy as np
from numba import njit
from numba.openmp import openmp_context as openmp

def axpy(x, y, a):
    y = a*x + y
    return y
    
@njit
def axpy_gpu(x, y, a, N):
    with openmp("target map(tofrom: y) map(to: x)"):
        for i in range(N):
           y[i] = a*x[i] + y[i]

    return y

N = 10
x = np.ones(N)
y = np.ones(N)
y = axpy(x, y, 2.0)
print(y)

x_gpu = np.ones(N)
y_gpu = np.ones(N)

y_gpu = axpy_gpu(x_gpu, y_gpu, 2.0, len(y))
print(y_gpu)

