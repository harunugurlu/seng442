#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#define NUM_THREADS 4

void *hello(void *args);

int main(int argc, char *argv[]) {
    pthread_t th[NUM_THREADS];

    for(int i = 0; i < NUM_THREADS; i++) {
        int *arg = malloc(sizeof(int));
        if(arg == NULL) {
            perror("error creating arg");
            return 1;
        }
        *arg = i;
        pthread_create(&th[i], NULL, hello, arg);
    }

    for(int i = 0; i < NUM_THREADS; i++) {
        pthread_join(th[i], NULL);
    }

    return 0;
}

void *hello(void* args) {
    int tid = *((int*)args);
    printf("Hello from thread %d\n", tid);
    free(args);
    return NULL;
}