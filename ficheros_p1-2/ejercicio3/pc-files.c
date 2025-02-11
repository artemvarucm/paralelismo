#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>
#include <unistd.h> 
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>

char *loadline(int fd);
void* producir(void* arg);
void* consumir(void* arg);

#define MAX_LINE_LENGTH 256
#define MAX_SBUFFER_SIZE 4
char* shared_buffer[MAX_SBUFFER_SIZE];

sem_t items; 
sem_t gaps;

pthread_mutex_t mutex;

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

	sem_init(&items, 0, 0); 
	sem_init(&gaps, 0, MAX_SBUFFER_SIZE);

	pthread_mutex_init(&mutex, NULL);
	pthread_create(&productor, NULL, producir, &fdIn);
	pthread_create(&consumidor, NULL, consumir, &fdOut);

	pthread_join(&productor, NULL);
	pthread_join(&consumidor, NULL);

	close(fdOut);
	if (input) {
		close(fdIn);
	}

	return 0;
}


void* producir(void* arg) {
	int fd = *(int*)arg;
	int i = 0;
	char *linea;

	while((linea = loadline(fd)) != NULL){
		char *p = malloc(strlen(linea) + 1);
		strcpy(p, linea);
		free(linea);

		sem_wait(&gaps);
		pthread_mutex_lock(&mutex);
		shared_buffer[i] = p;
		pthread_mutex_unlock(&mutex);
		sem_post(&items);

		i = (i + 1) % MAX_SBUFFER_SIZE;
	}

	sem_wait(&gaps);
	shared_buffer[i] = NULL;
	sem_post(&items);

    return NULL;
}


void* consumir(void* arg) {
	int fd = *(int*)arg;
	int i = 0;
	char *linea;
	
	while (1) {
		sem_wait(&items);
		pthread_mutex_lock(&mutex);
		linea = shared_buffer[i];
        

		if (linea==NULL){
			pthread_mutex_unlock(&mutex); 
            sem_post(&items);
			break;
		}

		int len = strlen(linea) + 1;
		if (write(fd, linea, len) < len) {
            perror("Error al escribir en el archivo");
            return 1;
        }

		free(linea);
		pthread_mutex_unlock(&mutex);
		sem_post(&gaps);

		i = (i + 1) % MAX_SBUFFER_SIZE;
	}

    return NULL;
}
char *loadline(int fd)
{
    int count = 0;
    char c;
    
    // Contar caracteres hasta encontrar '\n'
    while (read(fd, &c, sizeof(char)) == 1 && c != '\n') {
        count++;
    }

    if (count == 0) {
        return NULL;
    }
    
    // Retroceder el puntero del archivo
    lseek(fd, -(count + 1), SEEK_CUR);
    
    // Reservar memoria para la cadena
    char *str = (char*)malloc(count + 1);
    
	if (str) {
		// Volvemos a leer
        read(fd, str, count + 1);
    	return str;
    }

	return NULL;	
}
