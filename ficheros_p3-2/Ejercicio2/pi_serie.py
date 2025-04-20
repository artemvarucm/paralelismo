import time 

def piFunc(NumSteps):
    step = 1.0/NumSteps
    sum = 0.0

    for i in range(NumSteps):
        x = (i+0.5)*step
        sum += 4.0/(1.0 + x*x)
    pi=step*sum
    return pi

startTime = time.time()
mypi = piFunc(100000000)
stopTime = time.time()
print(f"pi={mypi:.4f} time={(stopTime-startTime):.2f}s")
