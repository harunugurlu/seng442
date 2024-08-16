#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>

#define N 100000
#define NUM_THREADS 4

// double factor = 1.0;
double sum = 0.0;
int count = 0;

pthread_mutex_t mutex;

void *calc_pi(void *args);

int main(int argc, char *argv[]) {

    pthread_t th[NUM_THREADS];
    pthread_mutex_init(&mutex, NULL);

    struct timespec start, end;
    double elapsed_time;
    
    clock_gettime(CLOCK_MONOTONIC, &start); // Start timing


    for(int i = 0; i < NUM_THREADS; i++) {
        long *rank = malloc(sizeof(long)); // Dynamically allocate memory to make sure each thread is pointing to a unique memory address.
        *rank = i;
        pthread_create(&th[i], NULL, calc_pi, rank);
    }

    for(int i = 0; i < NUM_THREADS; i++) {
        pthread_join(th[i], NULL);
    }

    printf("pi is %lf\n", (4*sum));

    clock_gettime(CLOCK_MONOTONIC, &end); // End timing

    elapsed_time = end.tv_sec - start.tv_sec;
    elapsed_time += (end.tv_nsec - start.tv_nsec) / 1000000000.0;
    
    printf("Total time taken: %f seconds\n", elapsed_time);

    

    return 0;
}

void *calc_pi(void *args) {
    long rank = *((long*)args);

    free(args);

    int start_index = rank * (N/NUM_THREADS);
    int end_index = start_index + (N/NUM_THREADS);

    double factor = (start_index % 2 == 0) ? 1.0 : -1.0; // Factor is +1 when i is even and -1 when i is odd
    double local_sum = 0.0;

    // printf("thread %ld is doing its work, start_index = %d, end_index = %d\n", rank, start_index, end_index);

    for(int i = start_index; i < end_index; i++, factor = -factor) {
        local_sum += factor/(2*i+1);
    }
    // printf("thread %ld is done, its sum is %lf\n", rank, sum);
    pthread_mutex_lock(&mutex);
    sum += local_sum;
    pthread_mutex_unlock(&mutex);
}