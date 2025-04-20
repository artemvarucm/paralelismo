from numba import njit
from numba.openmp import openmp_context as openmp
from numba.openmp import omp_get_num_threads, omp_set_num_threads, omp_get_wtime

MIN_BLK = 1024
@njit
def piComp(Nstart, Nfinish, step):
    iblk = Nfinish-Nstart
    if(iblk<MIN_BLK):
        sum = 0.0
        for i in range(Nstart,Nfinish):
            x= (i+0.5)*step
            sum += 4.0/(1.0 + x*x)
    else:
        sum1 = 0.0
        sum2 = 0.0
        with openmp ("task shared(sum1)"):
            sum1 = piComp(Nstart, Nfinish-iblk/2,step)
        with openmp ("task shared(sum2)"):
            sum2 = piComp(Nfinish-iblk/2,Nfinish,step)
        with openmp ("taskwait"):
            sum = sum1 + sum2
    return sum
    
@njit
def piFunc(NumSteps):
    step = 1.0/NumSteps
    sum = 0.0
    with openmp ("parallel"):
        with openmp ("single"):
            sum = piComp(0,NumSteps,step)
    pi = step*sum
    return step*sum

startTime = omp_get_wtime()
mypi = piFunc(100000000)
stopTime = omp_get_wtime()
print(f"pi={mypi:.4f} time={(stopTime-startTime):.2f}s")

