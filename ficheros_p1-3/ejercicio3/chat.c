#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>
#include <pthread.h>
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


int main(int argc, char* argv[])
{
	/* To be completed */
	return 0;
}
