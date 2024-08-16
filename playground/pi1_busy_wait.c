#include <stdio.h>
#include <pthread.h>
#include <time.h>

#define N 100000
#define NUM_THREADS 4

double factor = 1.0;
double sum = 0.0;
int count = 0;

void *calc_pi(void *args);

int main(int argc, char *argv[]) {

    pthread_t th[NUM_THREADS];

    struct timespec start, end;
    double elapsed_time;
    
    clock_gettime(CLOCK_MONOTONIC, &start); // Start timing


    for(int i = 0; i < NUM_THREADS; i++) {
        long rank = i;
        pthread_create(&th[i], NULL, calc_pi, &rank);
    }

    for(int i = 0; i < NUM_THREADS; i++) {
        double th_sum = 0;
        pthread_join(th[i], (void **)&th_sum);
        printf("thread sum: %lf\n", sum);
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
    printf("thread id: %ld\n", rank);

    while(count != rank);
    int start_index = rank * (N/NUM_THREADS);
    int end_index = start_index + (N/NUM_THREADS);
    printf("count = %d, thread %ld is doing its work, start_index = %d, end_index = %d\n", count, rank, start_index, end_index);
    for(int i = start_index; i < end_index; i++, factor = -factor) {
        sum += factor/(2*i+1);
    }
    printf("thread %ld is done, its sum is %lf\n", rank, sum);
    count++;

    return &sum;
}