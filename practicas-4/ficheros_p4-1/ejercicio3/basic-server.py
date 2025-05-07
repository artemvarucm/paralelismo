#!/usr/bin/env python3

from socket import *
import pandas as pd
import click

CITIES = ['Madrid', 'Rome', 'Berlin', 'Paris']
MONTHS = [str(m) for m in range(1, 13)]

#Leer dataframe y calcular medias
temperaturas = pd.read_csv("./temperatures.csv")
temperaturas["mes"] = pd.to_datetime(temperaturas["datetime"]).dt.month.astype(str)
media_temperaturas = temperaturas.groupby(["name", "mes"])["temp"].mean().reset_index()
temperaturas_diccionario = media_temperaturas.pivot_table(index="name", columns="mes", values="temp").to_dict(orient="index")

def manejar_cliente(conn, addr):
    data = conn.recv(1024).decode("utf-8") ## receive data from client
    if data.strip() == "END":
        print(F"Closing connection with {addr}")
        conn.send(("Closing connection\n").encode("utf-8"))
        conn.close()
        return False
    else:
        print(f"Received: {data}", end="")
        data_split = data.split(":")
        if len(data_split) != 2:
            respuesta = "Wrong format, should be city:month\n"
        elif data_split[1].strip() not in MONTHS:
            respuesta = "Wrong month\n"
        elif data_split[0] not in CITIES:
            respuesta = "Unknown city\n"
        else:
            respuesta = f"{str(temperaturas_diccionario[data_split[0]][data_split[1].strip()])}\n"
        conn.send(respuesta.encode("utf-8")) 
        return True

@click.command()
@click.argument('host', default="0.0.0.0")
@click.argument('puerto', default=10000, type=int)

def main(host, puerto):
    s = socket(AF_INET, SOCK_STREAM)
    s.setsockopt(SOL_SOCKET, SO_REUSEADDR, 1)
    s.bind((host, puerto))
    s.listen(1)
    print(f"Server listening on {host}:{puerto}")
    try:
        (conn, addr) = s.accept() # returns new socket and addr. client
        print(f"Accepted connection from {addr}")
        print(f"Handling connection from {addr}")
    
        while True: # forever
            activo = manejar_cliente(conn, addr)
            if not activo:
                (conn, addr) = s.accept()
                print(f"Accepted connection from {addr}")
                print(f"Handling connection from {addr}")
    finally:
        s.close()
        print("Server socket closed.")
        

if __name__ == "__main__":
    main()