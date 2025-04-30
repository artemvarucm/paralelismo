#!/usr/bin/env python
import threading
import time

# Ejemplo con Lock (cerrojo)
lock = threading.Lock()
contador = 0

def incrementar():
    global contador
    for _ in range(10000):
        lock.acquire()
        contador += 1
        lock.release()

h1 = threading.Thread(target=incrementar)
h2 = threading.Thread(target=incrementar)
h1.start()
h2.start()
h1.join()
h2.join()
print("Contador con Lock:", contador)