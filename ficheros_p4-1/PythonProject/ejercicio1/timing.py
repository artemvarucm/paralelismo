#!/usr/bin/env python
import time
from threading import Thread
from multiprocessing import Process

N = 5

#def tarea():
#    time.sleep(1)

"""
1.
Tiempo con hilos: 1.0080153942108154
Tiempo con procesos: 1.0276131629943848
2.
Tiempo con hilos: 1.5023441314697266
Tiempo con procesos: 0.4603893756866455
"""
def tarea():
    suma = 0
    for i in range(10**7):
        suma += i

def program():
    # Con hilos
    inicio = time.time()
    hilos = [Thread(target=tarea) for _ in range(N)]
    [t.start() for t in hilos]
    [t.join() for t in hilos]
    print("Tiempo con hilos:", time.time() - inicio)

    # Con procesos
    inicio = time.time()
    procesos = [Process(target=tarea) for _ in range(N)]
    [p.start() for p in procesos]
    [p.join() for p in procesos]
    print("Tiempo con procesos:", time.time() - inicio)
    
if __name__ == '__main__':
    program()