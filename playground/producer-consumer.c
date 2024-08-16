#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>

#define NUM_THREADS 2
#define BUFFER_SIZE 10
#define ITEMS 100

pthread_mutex_t mutex;
sem_t semFull; // How many slots are full
sem_t semEmpty; // How many slots are empty --> 

int in = 0;
int out = 0;

void *producer(void *args);
void *consumer(void* args);

int buffer[BUFFER_SIZE] = {0};

int producedItems[100] = {0};
int consumedItems[100] = {0};

int main(int argc, char *argv[]) {
    pthread_t th[NUM_THREADS];
    pthread_mutex_init(&mutex, NULL);
    sem_init(&semFull, 0, 0);
    sem_init(&semEmpty, 0, BUFFER_SIZE);

    srand(time(NULL));

    for(int i = 0; i < NUM_THREADS; i++) {
        if(i % 2 == 0) {
            pthread_create(&th[i], NULL, producer, NULL);
        }
        else {
            pthread_create(&th[i], NULL, consumer, NULL);
        }
    }

    for(int i = 0; i < NUM_THREADS; i++) {
        if(pthread_join(th[i], NULL) != 0) {
            perror("Thread join failed");
        }
    }
    for(int i = 0; i < 100; i++) {
        if(producedItems[i] != consumedItems[i]) {
            printf("Mismatch found for item %d: Produced %d, Consumed %d\n", i, producedItems[i], consumedItems[i]);
            return 1;
        }
    }
    printf("perfecto\n");
}

void *producer(void *args) {
    int count = 0;
    while(count < ITEMS) {
        int item = rand() % 100;
        sem_wait(&semEmpty); // Wait for an empty slot
        pthread_mutex_lock(&mutex);
        buffer[in] = item;
        in = (in+1)%BUFFER_SIZE;
        printf("producer produced: %d\n", item);
        producedItems[item]++;
        pthread_mutex_unlock(&mutex);
        sem_post(&semFull); // Increase full slots by 1
        count++;
    }
}

void *consumer(void *args) {
    int count = 0;
    while(count < ITEMS) {
        int item = 0;
        sem_wait(&semFull); // Wait for a full slot
        pthread_mutex_lock(&mutex);
        item = buffer[out];
        out = (out+1)%BUFFER_SIZE;
        consumedItems[item]++;
        printf("consumer consumed: %d\n", item);
        pthread_mutex_unlock(&mutex);
        sem_post(&semEmpty); // Increase the empty slots by 1
        count++;
    }
}