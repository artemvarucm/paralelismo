from numba import njit
import numpy as np
from numba.openmp import openmp_context as openmp
from numba.openmp import omp_get_thread_num, omp_get_num_threads, omp_get_wtime

NTHREADS = 8
@njit
def piFunc(NumSteps):
    step = 1.0/NumSteps
    partialSums = np.zeros(NTHREADS)
    with openmp("parallel shared(partialSums) private(threadID,numThrds,i,x,localSum) num_threads(8)"):
        threadID = omp_get_thread_num()
        numThrds = omp_get_num_threads()
        localSum = 0.0
        for i in range(threadID, NumSteps, numThrds):
            x = (i+0.5)*step
            localSum = localSum + 4.0/(1.0 + x*x)
        partialSums[threadID] = localSum
    return step*np.sum(partialSums)

startTime = omp_get_wtime()
mypi = piFunc(100000000)
stopTime = omp_get_wtime()
print(f"pi={mypi:.4f} time={(stopTime-startTime):.2f}s")
