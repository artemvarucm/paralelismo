#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>
#include <unistd.h> 
#include <pthread.h>
#include <semaphore.h>

#define N_THREADS 2
void mostrar_vector();
void *insertar(void *arg);

int *g_vector;
int tam;
sem_t semaforos[N_THREADS];
pthread_t hilos[N_THREADS];

int main(int argc, char* argv[])
{
	int opt;
	int tam_specified = 0;
	while((opt = getopt(argc, argv, "n:")) != -1) {
		switch(opt) {
			case 'n':
				tam_specified = 1;
				tam = strtol(optarg, NULL, 10); // base 10, no guardamos el caracter despues del numero
				break;
			default:
				exit(EXIT_FAILURE);
		}
	}
	
	if (!tam_specified) {
		fprintf(stderr,"Uso: %s -n <tamaño>\n",argv[0]);	
		return 1;
	} else if (tam <= 0) {
		fprintf(stderr,"El tamaño tiene que ser un entero positivo\n");	
		return 1;
	}

	g_vector = (int *)malloc(tam * sizeof(int));
	if (!g_vector) {
        perror("Error al reservar memoria");
        return 1;
    }

	int i;
	for(i=0; i < N_THREADS; i++) 
		sem_init(&semaforos[i], 0, i == 0 ? 1 : 0);

	for(i=0; i < N_THREADS; i++)
		pthread_create(&hilos[i], NULL, insertar, (void*)i);
	
	for(i=0; i < N_THREADS; i++)
		pthread_join(hilos[i],NULL);


	mostrar_vector();
	
	for(i=0; i < N_THREADS; i++) 
		sem_destroy(&semaforos[i]);

	free(g_vector);
	return 0;
}

void mostrar_vector() {
	for (int i = 0; i < tam; i++) {
        printf("%d%s", g_vector[i], (i < tam - 1) ? "," : "\n");
    }
}

void *insertar(void* i) {
	int offset = (int)i;
	int next = offset == N_THREADS - 1 ? 0 : offset + 1;

    for (int i = offset; i < tam; i += N_THREADS) {
        sem_wait(&semaforos[offset]);
		//printf("Thread %d escribiendo\n", offset);
        g_vector[i] = i + 1;
        sem_post(&semaforos[next]);
    }
    return NULL;
}
