#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#define NUM_HILOS 5

typedef struct {
    int numero_hilo;
    char prioridad;
} HiloInfo;


void *thread_usuario(void *arg)
{
    HiloInfo* info = (HiloInfo*)arg;
    pthread_t id = pthread_self();

    printf("Id del Hilo: %lu, Número de hilo: %d, Prioridad: %c\n", id, info->numero_hilo, info->prioridad);

    free(info);
    return NULL;
}

int main(int argc, char* argv[])
{
    pthread_t* hilos = malloc(NUM_HILOS * sizeof(pthread_t));

    for (int i = 0; i < NUM_HILOS; i++) {
        HiloInfo* info = malloc(sizeof(HiloInfo));

        info->numero_hilo = i;
        info->prioridad = i % 2 == 0 ? 'P' : 'N';  // Los hilos con número par son prioritarios

        if (pthread_create(&hilos[i], NULL, thread_usuario, (void*)info) != 0) {
            perror("Error al crear el hilo");
            return 1;
        }
    }

    // Esperamos a que todos los hilos terminen
    for (int i = 0; i < NUM_HILOS; i++) {
        if (pthread_join(hilos[i], NULL) != 0) {
            perror("Error al esperar el hilo");
            return 1;
        }
    }

    free(hilos);
    return 0;
}