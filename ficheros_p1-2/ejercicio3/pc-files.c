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

pthread_mutex_t mutex;
pthread_cond_t cond_prod, cond_cons;


pthread_t consumidor;
pthread_t productor;

int ridx=0;
int widx=0;
int nr_items=0;

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
	char *linea;

	pthread_mutex_lock(&mutex);

	while(nr_items==MAX_SBUFFER_SIZE){
		pthread_cond_wait(&cond_prod, &mutex);
	}

	while((linea = loadline(fd)) != NULL){
		char *p = malloc(strlen(linea) + 1);
		strcpy(p, linea);

		shared_buffer[widx] = p;

		free(linea);

		widx = (widx + 1) % MAX_SBUFFER_SIZE;
		nr_items++;
	}

	shared_buffer[widx] = NULL;
	
	pthread_cond_signal(&cond_cons);
	pthread_mutex_unlock(&mutex);

    return NULL;
}


void* consumir(void* arg) {
	int fd = *(int*)arg;
	char *linea;
	
	while (1) {
		pthread_mutex_lock(&mutex);
		while(nr_items==0){
			pthread_cond_wait(&cond_cons, &mutex);
		}
		linea = shared_buffer[ridx];
        
		if (linea==NULL){
			pthread_mutex_unlock(&mutex); 
			pthread_cond_signal(&cond_prod);
			break;
		}

		int len = strlen(linea) + 1;
		if (write(fd, linea, len) < len) {
            perror("Error al escribir en el archivo");
            return 1;
        }

		free(linea);

		ridx = (ridx + 1) % MAX_SBUFFER_SIZE;
		nr_items--;

		pthread_cond_signal(&cond_prod);
		pthread_mutex_unlock(&mutex);
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
