#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>

int count = 0;
sem_t count_sem;
sem_t barrier_sem;

void *thred_work(void *args) {

}

int main(int argc, char *argv[]) {
    sem_init(&count_sem, 0, 1);
    sem_init(&count_sem, 0, 0);
    
    for(int i = 0; i < 4; i++) {
        
    }
}