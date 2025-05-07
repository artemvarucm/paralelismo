#!/usr/bin/env python
from threading import Thread

class MiTarea(Thread):  # o Process
    def __init__(self):
        super().__init__()

    def run(self):
        print("Ejecutando tarea")

if __name__ == '__main__':
	thread = MiTarea()
	thread.start()
	thread.join()