#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>
#include <unistd.h> 
#include <pthread.h>
#include <fcntl.h>

void* producir(void* arg);
void* consumir(void* arg);
char* consumirLinea();
void producirLinea(char* linea);

#define MAX_LINE_LENGTH 256
#define MAX_SBUFFER_SIZE 4
char* shared_buffer[MAX_SBUFFER_SIZE];

pthread_mutex_t lock;
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

	FILE* fdIn = stdin;
	if (input) {
		fdIn = fopen(input, "r");
		if (fdIn == NULL) {
			perror("Error al abrir el archivo de entrada");
			return 1;
		}
	}

	FILE* fdOut = fopen(output, "w");
    if (fdOut == NULL) {
        perror("Error al abrir el archivo de salida");
        return 1;
    }

	if (pthread_mutex_init(&lock, NULL) != 0) { 
        perror("Mutex init has failed"); 
        return 1; 
    } 

	pthread_create(&productor, NULL, producir, fdIn);
	pthread_create(&consumidor, NULL, consumir, fdOut);

	pthread_join(productor, NULL);
	pthread_join(consumidor, NULL);

	pthread_mutex_destroy(&lock);

	fclose(fdOut);
	if (input) {
		fclose(fdIn);
	}

	return 0;
}

void* producir(void* arg) {
	FILE* fd = (FILE*) arg;
	char* linea = (char* )malloc(MAX_LINE_LENGTH);

	while(fgets(linea, MAX_LINE_LENGTH, fd) != NULL){
		//printf("%s", linea);
		producirLinea(linea);
	}

	producirLinea(NULL);

	// liberamos memoria
	free(linea);
    return NULL;
}


void* consumir(void* arg) {
	FILE* fd = (FILE*) arg;
	while(1) {
		char* linea = consumirLinea();
		if (linea == NULL) {
			break;
		} else {
			int len = strlen(linea) + 1;
			if (fwrite(linea, sizeof(char), len, fd) < len) {
				perror("Error al escribir en el archivo");
				exit(1);
			}

			free(linea);
		}
	}

    return NULL;
}

void producirLinea(char* linea) {
	pthread_mutex_lock(&lock);
	while(nr_items == MAX_SBUFFER_SIZE){
		pthread_cond_wait(&cond_prod, &lock);
	}
	//printf("Produce %s", linea);
	if (linea == NULL) {
		shared_buffer[widx] = NULL;
	} else {
		int len = strlen(linea) + 1;
		shared_buffer[widx] = (char* )malloc(len);
		strcpy(shared_buffer[widx], linea);
	}

	widx = (widx + 1) % MAX_SBUFFER_SIZE;
	nr_items++;
	
	pthread_cond_signal(&cond_cons);
	pthread_mutex_unlock(&lock);
}

char* consumirLinea() {
	pthread_mutex_lock(&lock);

	while(nr_items==0){
		pthread_cond_wait(&cond_cons, &lock);
	}
	char *linea = NULL;
	
	if (shared_buffer[ridx] != NULL) {
		int len = strlen(shared_buffer[ridx]) + 1;
		linea = (char* )malloc(len);
		strcpy(linea, shared_buffer[ridx]);
		//printf("Consume %s", linea);

		free(shared_buffer[ridx]);
	}

	ridx = (ridx + 1) % MAX_SBUFFER_SIZE;
	nr_items--;
	
	pthread_cond_signal(&cond_prod);
	pthread_mutex_unlock(&lock);

	return linea;
}