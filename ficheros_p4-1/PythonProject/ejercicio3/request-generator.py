#!/usr/bin/env python3
import sys
import socket
import threading
import random
import time

# Lista de ciudades válidas y meses válidos
CITIES = ['Madrid', 'Rome', 'Berlin', 'Paris']
MONTHS = list(range(1, 13))

# Variables compartidas (añadir lo necesario)
finished = False
request_count = 0

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
        ## while not finished:
        ##      Realizar petición
        ##      Recoger respuesta
        ##      Incrementar contador request_count de forma segura    
        ##      Introducir retardo indicado por el usuario

    except Exception as e:
        print(f"Error en cliente: {e}")


def main():
    global finished, request_count
   
    # Validar número de argumentos Parsear argumentos
    # ... TODO ... 
    
    ## Registrar tiempo inicial
    start_time = time.time()
    
    # Lanzar hilos cliente
    # ... TODO ... 
  
    # Esperar T segundos e indicar terminacion
    #time.sleep(T)
    #finished = True

    # Esperar a que todos los hilos terminen
    #for thread in threads:
    #    thread.join()
    
    ## Registrar tiempo final y tiempo transcurrido
    end_time = time.time()
    duration = end_time - start_time
    
    ## Calcular tasa de peticiones por segundo ...
    # ... TODO ... 

if __name__ == "__main__":
    main()
