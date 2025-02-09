#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>
#include <unistd.h> 
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>

void* producir(void* arg);
void* consumir(void* arg);

#define MAX_LINE_LENGTH 256
#define MAX_SBUFFER_SIZE 4
char* shared_buffer[MAX_SBUFFER_SIZE];

pthread_t consumidor;
pthread_t productor;

int main(int argc, char* argv[])
{
	int opt;
	char *input = NULL;
	char *output = NULL;
	while((opt = getopt(argc, argv, "i:o:")) != -1) {
		switch(opt) {
			case 'i':
				input = optarg;
				break;
			case 'o':
				output = optarg;
				break;
			default:
				exit(EXIT_FAILURE);
		}
	}
	
	if (!output) {
		output = "out.txt";
	}

	int fdIn = STDIN_FILENO;
	if (input) {
		fdIn = open(input, O_RDONLY);
		if (fdIn < 0) {
			perror("Error al abrir el archivo de entrada");
			return 1;
		}
	}

	int fdOut = open(output, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fdOut < 0) {
        perror("Error al abrir el archivo de salida");
        return 1;
    }

	pthread_create(&productor, NULL, producir, (void *)fdIn);
	pthread_create(&consumidor, NULL, consumir, (void *)fdOut);

	pthread_join(&productor, NULL);
	pthread_join(&consumidor, NULL);

	close(fdOut);
	if (input) {
		close(fdIn);
	}

	return 0;
}

void* producir(void* arg) {
	int fd = (int) arg;
	// completar
    return NULL;
}


void* consumir(void* arg) {
	int fd = (int) arg;
	int i = 0;
	while (shared_buffer[i] != NULL) {
		int len = strlen(shared_buffer[i]) + 1;
		if (write(fd, shared_buffer[i], len) < len) {
            perror("Error al escribir en el archivo");
            return 1;
        }

		free(shared_buffer[i]);
		i = (i + 1) % MAX_SBUFFER_SIZE;
	}

    return NULL;
}