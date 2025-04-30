#!/usr/bin/env python
import threading
import time
import random

buffer = []
MAX_BUFFER = 5

lock = threading.Lock()
cond_productor = threading.Condition(lock)
cond_consumidor = threading.Condition(lock)

def productor():
    global buffer
    for i in range(10):
        item = f"item-{i}"
        lock.acquire()
        while len(buffer) >= MAX_BUFFER:
            cond_productor.wait()
        buffer.append(item)
        print(f"Producido: {item} (buffer={len(buffer)})")
        cond_consumidor.notify()
        lock.release()
        time.sleep(random.uniform(0.1, 0.4))

def consumidor():
    global buffer
    for _ in range(10):
        lock.acquire()
        while not buffer:
            cond_consumidor.wait()
        item = buffer.pop(0)
        print(f"Consumido: {item} (buffer={len(buffer)})")
        cond_productor.notify()
        lock.release()
        time.sleep(random.uniform(0.2, 0.5))

prod = threading.Thread(target=productor)
cons = threading.Thread(target=consumidor)

prod.start()
cons.start()
prod.join()
cons.join()
