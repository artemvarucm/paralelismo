#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>
#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>
#define MAX_CHARS_MSG   128

typedef enum  {
    NORMAL_MSG,	 /* Mensaje para transferir lineas de la 
						 entre 	ambos usuarios del chat */
    USERNAME_MSG,  /* Tipo de mensaje reservado para enviar 
						el nombre de usuario al otro extremo*/    
    END_MSG	 /* Tipo de mensaje que se envía por el FIFO 
				cuando un extremo finaliza la comunicación */
} message_type_t;

struct chat_message{
     char contenido[MAX_CHARS_MSG]; //Cadena de caracteres (acabada en '\0) 
     message_type_t type;	
};

pthread_t emisor;
pthread_t receptor;

int fifo_envio, fifo_receptor;

char *usuario, *nombre_otro_usuario;

void *enviar();
void *recibir();

int main(int argc, char* argv[])
{
     if (argc < 4) {
          fprintf(stderr, "Uso: %s <usuario> <ruta-fifo-envío> <ruta-fifo-recepción>\n", argv[0]);
          return 1;
     }

     //Abrimos los archivos
     usuario = malloc(strlen(argv[1]) + 1);
     strcpy(usuario, argv[1]);
     fifo_envio = open(argv[2], O_WRONLY);
     fifo_receptor = open(argv[3], O_RDONLY);

     if (fifo_envio == -1 || fifo_receptor == -1) {
          perror("Error al abrir los FIFOs");
          return 1;
     }

     // Crear los dos hilos
     pthread_create(&emisor, NULL, enviar, NULL);
     pthread_create(&receptor, NULL, recibir, NULL);

     pthread_join(emisor, NULL);
     pthread_join(receptor, NULL);

     free(usuario);
     free(nombre_otro_usuario);

       
	return 0;
}

void *enviar() {
     struct chat_message msg;
     msg.type = USERNAME_MSG;
     strncpy(msg.contenido, usuario, MAX_CHARS_MSG);
     
     write(fifo_envio, &msg, sizeof(msg));
     
     while (fgets(msg.contenido, MAX_CHARS_MSG, stdin) != NULL) {
         msg.type = NORMAL_MSG;
         write(fifo_envio, &msg, sizeof(msg));
     }
     
     msg.type = END_MSG;
     write(fifo_envio, &msg, sizeof(msg));
     close(fifo_envio);
     return NULL;
 }

 void *recibir() {
     struct chat_message msg;
     
     if (read(fifo_receptor, &msg, sizeof(msg)) > 0 && msg.type == USERNAME_MSG) {
          strncpy(nombre_otro_usuario, msg.contenido, MAX_CHARS_MSG); 
          printf("%s se ha conectado.\n", nombre_otro_usuario);
     }
     
     while (1) {
         if (read(fifo_receptor, &msg, sizeof(msg)) > 0) {
             if (msg.type == NORMAL_MSG) {
                 printf("%s dice: %s", nombre_otro_usuario, msg.contenido);
             } else if (msg.type == END_MSG) {
                 printf("%s ha salido del chat.\n", nombre_otro_usuario);
                 break;
             }
         }
     }
     close(fifo_receptor);
     return NULL;
 }