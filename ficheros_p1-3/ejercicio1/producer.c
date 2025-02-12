#include <sys/mman.h>
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <semaphore.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_BUFFER          4    /* buffer size */
#define DATA_TO_PRODUCE  16    /* # elements to produce */
sem_t *elements;  /* # of elements in the buffer */
sem_t *gaps;     /* # of free gaps in the buffer */

void Producer(int *buffer)
{
    int pos = 0;  /* write index */
    int item;     /* data to produce */
    int i;

    for(i=0; i < DATA_TO_PRODUCE; i++ ) {
        item = i; /* Producing number */
        sem_wait(gaps);
        buffer[pos] = item;
        pos = (pos + 1) % MAX_BUFFER;
        sem_post(elements);
        printf("Producing %d\n",i);
        usleep(500000);
    }
}

void main(int argc, char *argv[]) {
    int shd;
    int *buffer;    /* shared buffer */

    /**
     *  To make debugging easier
     * (destroy semaphore from previous failed runs)
     **/
    sem_unlink("ELEMENTS");
    sem_unlink("GAPS");

    /* the producer creates the file */
    shd = open("/tmp/BUFFER", O_CREAT| O_RDWR | O_TRUNC, 0777);

    if (shd==-1) {
        perror("open");
        exit(EXIT_FAILURE);
    }


    if (ftruncate(shd, MAX_BUFFER * sizeof(int)) == -1) {
        perror("ftruncate");
        close(shd); // Make sure to close file descriptor
        exit(EXIT_FAILURE);
    }

    /* Maps the file into the process address space */
    buffer = (int*) mmap(NULL, MAX_BUFFER * sizeof(int),
                         PROT_WRITE| PROT_READ, MAP_SHARED, shd, 0);

    if (buffer==MAP_FAILED) {
        perror("mmap");
        close(shd);
        exit(EXIT_FAILURE);
    }

    /* The producer creates the semaphores */
    elements = sem_open("ELEMENTS", O_CREAT, 0700, 0);

    if (elements == SEM_FAILED) {
        perror("sem_open (elements)");
        munmap(buffer, MAX_BUFFER * sizeof(int));
        close(shd);
        exit(EXIT_FAILURE);
    }

    gaps =    sem_open("GAPS", O_CREAT, 0700, MAX_BUFFER);

    if (gaps == SEM_FAILED) {
        perror("sem_open (gaps)");
        sem_close(elements);
        sem_unlink("ELEMENTS");
        munmap(buffer, MAX_BUFFER * sizeof(int));
        close(shd);
        exit(EXIT_FAILURE);
    }

    /* core producer's code */
    Producer(buffer);

    /* Unmap shared buffer */
    munmap(buffer, MAX_BUFFER * sizeof(int));
    close(shd);        /* close the shared memory region */
    unlink("/tmp/BUFFER");  /* delete the shared memory region */

    sem_close(elements);
    sem_close(gaps);
    sem_unlink("ELEMENTS");
    sem_unlink("GAPS");
}