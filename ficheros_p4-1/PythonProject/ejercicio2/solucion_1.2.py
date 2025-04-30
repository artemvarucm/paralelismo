#!/usr/bin/env python
import threading
import time

# practica 1.2, ejercicio 2 A reimplementado

N_THREADS = 2
semaforos = [threading.Semaphore(0) for i in range(N_THREADS)]
semaforos[0].release()
tam = 10

def insertar(i):
    offset = i
    next = (offset + 1) % N_THREADS
    for i in range(i, tam, N_THREADS):
        semaforos[offset].acquire()
        g_vector.append(i+1)
        semaforos[next].release()


g_vector = [] # inicializamos vector global
threads=[]

for i in range(N_THREADS):
    t=threading.Thread(target=insertar, args=(i,))
    t.start()
    threads.append(t)

for thread in threads:
    thread.join()

print(g_vector)