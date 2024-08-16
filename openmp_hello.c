#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

void hello();

int main(int argc, char** argv) {
    int thread_count = atoi(argv[1]);
    printf("thread_num %d\n", thread_count);

    #pragma omp parallel num_threads(thread_count)
    hello();
}

void hello() {
    int my_rank = omp_get_thread_num();
    int thread_count = omp_get_num_threads();

    printf("Hello from thread %d of %d\n", my_rank, thread_count);
}