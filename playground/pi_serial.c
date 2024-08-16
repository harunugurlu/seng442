#include <stdio.h>
#include <pthread.h>
#include <time.h>

#define N 100000000


double factor = 1.0;
double sum = 0.0;


double calc_pi();

int main(int argc, char *argv[]) {

    struct timespec start, end;
    double elapsed_time;
    
    clock_gettime(CLOCK_MONOTONIC, &start); // Start timing

    calc_pi();

    printf("pi is %lf\n", (4*sum));

    clock_gettime(CLOCK_MONOTONIC, &end); // End timing

    elapsed_time = end.tv_sec - start.tv_sec;
    elapsed_time += (end.tv_nsec - start.tv_nsec) / 1000000000.0;
    
    printf("Total time taken: %f seconds\n", elapsed_time);

    return 0;
}

double calc_pi() {
    for(int i = 0; i < N; i++, factor = -factor) {
        sum += factor/(2*i+1);
    }
}