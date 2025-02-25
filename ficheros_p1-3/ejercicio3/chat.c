#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>
#include <pthread.h>
#include <sys/types.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>
#include <errno.h>
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

// contiene los datos que se pasan a los threads
struct thread_data {
     char* path_fifo;
     char* username; 
};

pthread_t sender;
pthread_t receiver;

void fifo_receive (struct thread_data* info);
void fifo_send (struct thread_data* info);
void write_fifo(int fd_fifo, struct chat_message * message, int size);
//void read_fifo(int fd_fifo, struct chat_message * message, int size);

int main(int argc, char* argv[])
{
     if (argc < 4) {
          fprintf(stderr,"Usage: %s <usuario> <ruta-fifo-emisor> <ruta-fifo-receptor>\n",argv[0]);	
          return 1;
     }

     struct thread_data sender_info = {argv[2], argv[1]};
     struct thread_data receiver_info = {argv[3], argv[1]};
     pthread_create(&sender, NULL, fifo_send, &sender_info);
	pthread_create(&receiver, NULL, fifo_receive, &receiver_info);

	pthread_join(sender, NULL);
	pthread_join(receiver, NULL);

	return 0;
}


void write_fifo(int fd_fifo, struct chat_message *message, int size) {
     int wbytes=write(fd_fifo,message,size);

     if (wbytes > 0 && wbytes!=size) {
          fprintf(stderr,"Can't write the whole register\n");
          exit(1);
     }else if (wbytes < 0){
          perror("Error when writing to the FIFO\n");
          exit(1);
     }
}

void fifo_send (struct thread_data* info) {
     struct chat_message message;
     int fd_fifo=0;
     const int size=sizeof(struct chat_message);

     fd_fifo=open(info->path_fifo,O_WRONLY);

     if (fd_fifo<0) {
          perror(info->path_fifo);
          exit(1);
     }
     printf("\33[2K\r");
     printf("Conexión de envío establecida!!\n");
     /* Bucle de envío de datos a través del FIFO
     - Leer de la entrada estandar hasta fin de fichero
     */
     message.type = USERNAME_MSG;
     strcpy(message.contenido, info->username);
     write_fifo(fd_fifo, &message, size);
     
     printf("> ");
     fflush(stdout);
     

     while(fgets(message.contenido, MAX_CHARS_MSG, stdin) != NULL) {
          message.type = NORMAL_MSG;
          write_fifo(fd_fifo, &message, size);

          printf("> ");
          fflush(stdout);
     }

     // enviamos mensaje EOF
     message.type = END_MSG;
     write_fifo(fd_fifo, &message, size);
     
     close(fd_fifo);
}



void fifo_receive (struct thread_data* info) {
  struct chat_message message;
  int fd_fifo=0;
  int bytes=0;
  const int size=sizeof(struct chat_message);
  char *nombre_interlocutor[MAX_CHARS_MSG];

  fd_fifo=open(info->path_fifo,O_RDONLY);

  if (fd_fifo<0) {
	perror(info->path_fifo);
	exit(1);
  }
  printf("\33[2K\r");
  printf("Conexión de recepción establecida!!\n");
  while((bytes=read(fd_fifo,&message,size))==size) {
     printf("\33[2K\r");
     if (message.type == NORMAL_MSG) {
          /* Write to stdout */
          printf("%s dice: %s", nombre_interlocutor, message.contenido);
     } else if (message.type == USERNAME_MSG) {
          strcpy(nombre_interlocutor, message.contenido);
     } else {
          printf("Conexión finalizada por %s!!\n", nombre_interlocutor);
          close(fd_fifo);
          exit(0);
     }
     printf("> ");
     fflush(stdout);
 }

  if (bytes > 0){
	fprintf(stderr,"Can't read the whole register\n");
	exit(1);
  }else if (bytes < 0) {
	fprintf(stderr,"Error when reading from the FIFO\n");
	exit(1);
  }
	
   close(fd_fifo);
}