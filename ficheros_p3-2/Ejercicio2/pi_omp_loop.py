from numba import njit
from numba.openmp import openmp_context as openmp
from numba.openmp import omp_get_wtime

@njit
def piFunc(NumSteps):
    step = 1.0/NumSteps
    sum = 0.0
    with openmp("parallel for private(x) reduction(+:sum)"):
        for i in range(NumSteps):
            x = (i+0.5)*step
            sum += 4.0/(1.0 + x*x)
    pi=step*sum
    return pi

startTime = omp_get_wtime()
mypi = piFunc(100000000)
stopTime = omp_get_wtime()
print(f"pi={mypi:.4f} time={(stopTime-startTime):.2f}s")
