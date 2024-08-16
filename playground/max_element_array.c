#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define MAX_ARRAY 16
#define NUM_THREADS 4

int arr[MAX_ARRAY] = {1, 5, 7, 20, 10, 28, 32, 2, 46, 784, 234, 123, 323, 19, 423, 49};

void *th_find_max(void* args);
int find_max(int *element);

int main(int argc, char *argv[]) {
    pthread_t th[NUM_THREADS];

    for(int i = 0; i < NUM_THREADS; i++) {
        int *arg = malloc(sizeof(int));
        if(arg == NULL) {
            perror("Error creating the argument");
            return 1;
        }
        *arg = i;
        pthread_create(&th[i], NULL, th_find_max, arg);
    }

    int max_elements[NUM_THREADS] = {0};

    for(int i = 0; i < NUM_THREADS; i++) {
        int *max;
        pthread_join(th[i], (void **)&max);
        max_elements[i] = *max;
        free(max);
    }

    for(int i = 0; i < NUM_THREADS; i++) {
        printf("one of the max_elements is %d\n", max_elements[i]);
    }

    int max_element = find_max(max_elements);

    printf("Maximum element of the array is %d\n", max_element);
    return 0;
}

void *th_find_max(void* args) {
    int tid = *((int*)args);
    free(args);

    int *max = malloc(sizeof(int)); // Dynamically allocate memory for max
    if (max == NULL) {
        perror("Failed to allocate memory for max");
        pthread_exit(NULL); // Exit the thread if memory allocation fails
    }


    int start_index = tid * (MAX_ARRAY / NUM_THREADS);
    int end_index = start_index + NUM_THREADS;

    *max = arr[start_index];

    printf("Thread %d doing its job, its start_index: %d\n", tid, start_index);

    for(int i = start_index; i < end_index; i++) {
        int curr = arr[i];
        //printf("max is %d\n", max);
        if(*max < curr) {
            *max = curr;
        }
    }

    return max;
}

int find_max(int *element) {
    int max = 0;
    for(int i = 0; i < NUM_THREADS; i++) {
        int curr = element[i];
        if(max < curr) {
            max = curr;
        }
    }
    return max;
}