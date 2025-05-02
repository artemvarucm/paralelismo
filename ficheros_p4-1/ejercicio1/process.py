#!/usr/bin/env python
from multiprocessing import Process

class MiTarea(Process):  # o Process
    def __init__(self):
        super().__init__()

    def run(self):
        print("Ejecutando tarea")

if __name__ == '__main__':
    process = MiTarea()
    process.start()
    process.join()