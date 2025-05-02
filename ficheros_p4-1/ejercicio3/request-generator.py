#!/usr/bin/env python3
import sys
import socket
from socket import *
import threading
import random
import time
import click

# Lista de ciudades válidas y meses válidos
CITIES = ['Madrid', 'Rome', 'Berlin', 'Paris']
MONTHS = list(range(1, 13))

# Variables compartidas (añadir lo necesario)
finished = False
request_count = 0
threads = []
lock = threading.Lock()

def generate_random_request():
    city = random.choice(CITIES)
    month = random.randint(1, 12)
    return f"{city}:{month}"

def client_thread(host, port, delay_ms):
    global finished, request_count

    try:
        print("Hilo arrancado")
        ## Todo 
        
        ## Crear socket y establecer conexión con el servidor
        s = socket(AF_INET, SOCK_STREAM)
        s.connect((host, port))
        while not finished:
            ##      Realizar petición
            s.send(generate_random_request().encode("utf-8"))
            ##      Recoger respuesta
            response = s.recv(1024).decode("utf-8")
            ##      Incrementar contador request_count de forma segura
            lock.acquire()
            request_count += 1
            lock.release()
            ##      Introducir retardo indicado por el usuario
            time.sleep(delay_ms / 1000)

    except Exception as e:
        print(f"Error en cliente: {e}")

@click.command()
@click.argument('host_y_puerto', default="0.0.0.0:10000")
@click.argument('total_time', default=20, type=int) # el tiempo total
@click.argument('n_threads', default=10, type=int) # el num. de threads
@click.argument('mili_delay', default=10, type=int) # el delay

def main(host_y_puerto, total_time, n_threads, mili_delay):
    global finished, request_count
    
    if len(host_y_puerto.split(":")) != 2:
        print("Formato incorrecto, debe ser host:port")
        exit(1)
    
    host, puerto = host_y_puerto.split(":")

    if host == "" or puerto == "":
        print("Formato incorrecto, debe ser host:port")
        exit(1)

    ## Registrar tiempo inicial
    start_time = time.time()
    
    # Lanzar hilos cliente
    for _ in range(n_threads):
        t = threading.Thread(target=client_thread, args=(host, int(puerto), mili_delay))
        t.start()
        threads.append(t)

    # Esperar T segundos e indicar terminacion
    time.sleep(total_time)
    finished = True

    # Esperar a que todos los hilos terminen
    for thread in threads:
        thread.join()
    
    ## Registrar tiempo final y tiempo transcurrido
    end_time = time.time()
    duration = end_time - start_time
    
    ## Calcular tasa de peticiones por segundo ...
    print(f"Peticiones totales: {request_count}")
    print(f"Duración: {round(duration, 2)} segundos")
    print(f"Tasa de peticiones: {round(request_count / duration, 2)} peticiones/segundo")

if __name__ == "__main__":
    main()