#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

double local_trap(int a, int b, int n);

int main(int argc, char** argv) {
    int thread_num = atoi(argv[1]);
    double global_result = 0.0;
    #pragma omp parallel num_threads(thread_num) \
    reduction(+: global_result)
    {
        global_result += local_trap(0, 24, 24);
    }

    printf("global_result %f\n", global_result);
}

double f(int x) {
    return x * x;
}

double local_trap(int a, int b, int n) {
    int thread_num = omp_get_thread_num();
    int num_threads = omp_get_num_threads();

    int h = (b-a) / n;

    int local_n = n / num_threads;
    int local_a = a + local_n * h * thread_num;
    int local_b = local_a + h * local_n;

    double approx = (f(local_a) + f(local_b)) / 2.0;

    for(int i = 1; i < local_n; i++) {
        int x_i = local_a + i*h;
        approx += f(x_i);
    }
    return approx * h;
}