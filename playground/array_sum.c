#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define MAX_ARRAY 16
#define NUM_THREADS 4

int arr[MAX_ARRAY] = {1, 5, 7, 20, 10, 28, 32, 2, 46, 784, 234, 123, 323, 19, 423, 49};
int arr_th_sum[NUM_THREADS] = {0};

pthread_barrier_t barrier;

void *arr_sum(void *rank);

int main(int argc, char *argv[]) {
    pthread_t tid[NUM_THREADS];

    struct timespec start, end;
    double elapsed_time;

    pthread_barrier_init(&barrier, NULL, NUM_THREADS);
    
    clock_gettime(CLOCK_MONOTONIC, &start); // Start timing

    for(int i = 0; i < NUM_THREADS; i++) {
        int *rank = malloc(sizeof(int)); // Dynamically allocate memory to make sure each thread is pointing to a unique memory address.
        *rank = i;
        if(pthread_create(&tid[i], NULL, arr_sum, rank) != 0) {
            perror("Error creating thraed");
            return 1;
        }
    }

    int th_sum_arr = 0;

    for(int i = 0; i < NUM_THREADS; i++) {
        int *th_sum;
        if(pthread_join(tid[i], (void **)&th_sum) != 0) {
            perror("Error joining threads");
            return 1;
        }
        th_sum_arr += *(int *)th_sum;
    }

    pthread_barrier_destroy(&barrier);

    int sum_arr = 0;

    for(int i = 0; i < MAX_ARRAY; i++) {
        sum_arr += arr[i];
    }

    if(sum_arr != th_sum_arr) {
        printf("sum_arr: %d th_sum_arr: %d, there is something wrong\n", sum_arr, th_sum_arr);
        return 0;
    }

    printf("sum_arr: %d th_sum_arr: %d, correct!\n", sum_arr, th_sum_arr);

    clock_gettime(CLOCK_MONOTONIC, &end); // End timing

    elapsed_time = end.tv_sec - start.tv_sec;
    elapsed_time += (end.tv_nsec - start.tv_nsec) / 1000000000.0;
    
    printf("Total time taken: %f seconds\n", elapsed_time);

    return 0;
}

void *arr_sum(void *rank) {
    int th_rank = *((int*)rank);
    free(rank);


    int start_index = th_rank * (MAX_ARRAY / NUM_THREADS);
    int end_index = start_index + (MAX_ARRAY / NUM_THREADS);

    int ret = pthread_barrier_wait(&barrier);
    printf("thread: %d is executing, ret: %d\n", th_rank, ret);
    if (ret == PTHREAD_BARRIER_SERIAL_THREAD) {
        printf("Last thread %d has reached the barrier and will be released first\n", th_rank);
    }

    
    int *sum = malloc(sizeof(int));
    if (sum == NULL) {
        perror("Failed to allocate memory for sum");
        exit(1);  // Exit if memory allocation fails
    }
    *sum = 0;  // Initialize sum to 0

    for(int i = start_index; i < end_index; i++) {
        *sum += arr[i];
    }

    return sum;
}