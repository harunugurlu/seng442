#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#define ARR_SIZE 10
#define NUM_THREADS 2

int arr[] = {1,2,3,4,5,6,7,8,9,10};
int sum = 0;
int sum_arr[NUM_THREADS];

void *array_sum(void* args);
void *array_sum_no_cs(void * args);

pthread_mutex_t mutex;

int main() {
    pthread_t th[2];
    pthread_mutex_init(&mutex, NULL);
    for(int i = 0; i < 2; i++) {
        int* rank = malloc(sizeof(int));
        *rank = i;
        pthread_create(&th[i], NULL, array_sum_no_cs, rank);
    }

    for(int i = 0; i < NUM_THREADS; i++) {
        pthread_join(th[i], NULL);
    }

    for(int i = 0; i < NUM_THREADS; i++) {
        sum += sum_arr[i];
    }

    printf("sum is %d\n", sum);
}

void *array_sum(void* args) {
    int rank = *((int*)args);
    int start_index = rank * ARR_SIZE/NUM_THREADS;
    int end_index = start_index + (ARR_SIZE/NUM_THREADS);
    // printf("rank %d, start_index %d, end_index %d\n", rank, start_index, end_index);
    int local_sum = 0;
    for(int i = start_index; i < end_index; i++) {
        local_sum += arr[i];
        // printf("local_sum is %d\n", local_sum);
    }
    pthread_mutex_lock(&mutex);
    sum += local_sum;
    pthread_mutex_unlock(&mutex);
}

void *array_sum_no_cs(void * args) {
    int rank = *((int*)args);
    int start_index = rank * ARR_SIZE/NUM_THREADS;
    int end_index = start_index + (ARR_SIZE/NUM_THREADS);
    // printf("rank %d, start_index %d, end_index %d\n", rank, start_index, end_index);
    for(int i = start_index; i < end_index; i++) {
        sum_arr[rank] += arr[i];
        // printf("local_sum is %d\n", local_sum);
    }
}