#include <sys/mman.h>
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <semaphore.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_BUFFER          4    /* buffer size */
#define DATA_TO_CONSUME  16    /* # elements to produce */
sem_t *elements;  /* # of elements in the buffer */
sem_t *gaps;     /* # of free gaps in the buffer */

void Consumer(int *buffer)
{
    int pos = 0;  /* read index */
    int i, item;

    for(i=0; i < DATA_TO_CONSUME; i++ )  {
        sem_wait(elements);
        item = buffer[pos];
        pos = (pos + 1) % MAX_BUFFER;
        sem_post(gaps);
        printf("Consuming %d\n", item);
    }
}

void main(int argc, char *argv[]) {
    int shd;
    int *buffer;    /* shared buffer */

    /* the consumer opens the file */
    shd = open("/tmp/BUFFER", O_RDONLY);

    if (shd==-1) {
        perror("open");
        exit(1);
    }

    /* Maps the file into the process address space */
    buffer = (int *) mmap(NULL, MAX_BUFFER * sizeof(int),
                          PROT_READ, MAP_SHARED, shd, 0);

    if (buffer==MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    /* Consumer opens semaphores */
    elements = sem_open("ELEMENTS", 0);
    if (elements == SEM_FAILED) {
        perror("sem_open (elements)");
        munmap(buffer, MAX_BUFFER * sizeof(int));
        close(shd);
        exit(EXIT_FAILURE);
    }

    gaps = sem_open("GAPS", 0);

    if (gaps == SEM_FAILED) {
        perror("sem_open (gaps)");
        sem_close(elements);
        munmap(buffer, MAX_BUFFER * sizeof(int));
        close(shd);
        exit(EXIT_FAILURE);
    }

    /* consumer's core processing */
    Consumer(buffer);

    /* unmap shared buffer */
    munmap(buffer,  MAX_BUFFER * sizeof(int));
    close(shd);   /* close shared memory object */

    /* close semaphores */
    sem_close(elements);
    sem_close(gaps);
}