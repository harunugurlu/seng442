#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

#define NUM_THREADS 4

sem_t barrier;
sem_t room;
int count = 0;

void *threads(void *args);

int main() {
    pthread_t th[NUM_THREADS];
    sem_init(&barrier, 0, 0);
    sem_init(&room, 0, 1);

    for(int i = 0; i < NUM_THREADS; i++) {
        int *rank = malloc(sizeof(int));
        *rank = i;
        pthread_create(&th[i], NULL, threads, rank);
    }

    for(int i = 0; i < NUM_THREADS; i++) {
        pthread_join(th[i], NULL);
    }



    return 0;
}

void *threads(void *args) {
    int rank = *((int*)args);
    sem_wait(&room);
    count += 1;
    printf("Thread %d has entered the room, room_count %d\n", rank, count);
    sem_post(&room);

    if (count == NUM_THREADS) {
        printf("Thread %d is unlocking the barrier\n", rank);
        sem_post(&barrier);
    }

    sem_wait(&barrier); // Wait at the barrier
    sem_post(&barrier); // Allow the next thread to proceed

    printf("Thread %d has passed the barrier\n", rank);

    return NULL;
}