#include <stdio.h> 
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#define NR_PHILOSOPHERS 5

pthread_t philosophers[NR_PHILOSOPHERS];

/** 
 * IMPORTANT NOTE
 * In philosophers_sem.c, these mutexes need to
 * be replaced with semaphores.
*/
pthread_mutex_t forks[NR_PHILOSOPHERS];

void init()
{
    int i;
    for(i=0; i<NR_PHILOSOPHERS; i++)
        pthread_mutex_init(&forks[i],NULL);
}

void think(int i) {
    printf("Philosopher %d thinking... \n" , i);
    sleep(random() % 10);
    printf("Philosopher %d stopped thinking!!! \n" , i);
}

void eat(int i) {
    printf("Philosopher %d eating... \n" , i);
    sleep(random() % 5);
    printf("Philosopher %d is not eating anymore!!! \n" , i);
}

void toSleep(int i) {
    printf("Philosopher %d sleeping... \n" , i);
    sleep(random() % 10);
    printf("Philosopher %d is awake!!! \n" , i);
}

void* philosopher(void* i)
{
    int nPhilosopher = (int)i;
    int right = nPhilosopher;
    int left = (nPhilosopher - 1 == -1) ? NR_PHILOSOPHERS - 1 : (nPhilosopher - 1);
    
    if (nPhilosopher == NR_PHILOSOPHERS - 1 ) {
        int tmp = right;
        right = left;
        left = tmp;
    }

    while(1)
    {
        
        think(nPhilosopher);
        
        /// TRY TO GRAB BOTH FORKS (right and left)
        pthread_mutex_lock(&forks[right]);
        pthread_mutex_lock(&forks[left]);

        eat(nPhilosopher);
        
        // PUT FORKS BACK ON THE TABLE
        pthread_mutex_unlock(&forks[right]);
        pthread_mutex_unlock(&forks[left]);
        toSleep(nPhilosopher);
   }

}

int main()
{
    init();
    unsigned long i;
    for(i=0; i<NR_PHILOSOPHERS; i++)
        pthread_create(&philosophers[i], NULL, philosopher, (void*)i);
    
    for(i=0; i<NR_PHILOSOPHERS; i++)
        pthread_join(philosophers[i],NULL);

    for(i=0; i<NR_PHILOSOPHERS; i++)
        pthread_mutex_destroy(&forks[i]);

    return 0;
} 
