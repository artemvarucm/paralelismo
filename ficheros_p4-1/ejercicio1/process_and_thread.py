#!/usr/bin/env python
import threading
import multiprocessing
import time

def worker(name):
    print(f"Inicio de {name}")
    time.sleep(2)
    print(f"Fin de {name}")


if __name__ == '__main__':
    # Hilo
    t = threading.Thread(target=worker, args=("Hilo",))
    # Proceso
    p = multiprocessing.Process(target=worker, args=("Proceso",))
    t.start()
    p.start()
    t.join()
    p.join()