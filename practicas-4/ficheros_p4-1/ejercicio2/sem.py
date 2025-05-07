#!/usr/bin/env python
import threading
import time

# Ejemplo con Semaphore
sem = threading.Semaphore(3)

def tarea_con_limite(id):
    sem.acquire()
    print(f"Hilo en seccion critica, {id}", flush=True)
    time.sleep(1)
    sem.release()

threads=[]
for i in range(6):
    t=threading.Thread(target=tarea_con_limite, args=(i,))
    t.start()
    threads.append(t)
    
for thread in threads:
    thread.join()