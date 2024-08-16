#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int sum(int a, int b, int n);
int f(int x);
void total_sum(int a, int b, int n, int* global_sum);

int main(int argc, char** argv) {
    int num_threads = atoi(argv[1]);
    int a = 0, b = 24, n = 24;
    int global_sum = 0;
    // int local_sum = 0;
    #pragma omp parallel num_threads(num_threads) 
    {

        int thread_num = omp_get_thread_num();
        int num_threads = omp_get_num_threads();
        int h = (b-a) / n;
        int local_n = n / num_threads;
        int local_a = a + h * local_n * thread_num;
        int local_b = local_a + local_n * h;

        int local_sum = sum(local_a, local_b, local_n);
        printf("thread %d has found local sum as %d\n", thread_num, local_sum);
        

        #pragma omp critical 
        {
            global_sum += local_sum;
        }
    }

    printf("total_sum is %d\n", global_sum);
}

int f(int x) {
    return x * x;
}

int sum(int a, int b, int n) {
    int h = (b-a) / n;
    int approx = (f(a) + f(b)) / 2;
    for(int i = 1; i < n; i++) {
        int x_i = a + i*h;
        approx += f(x_i);
    }
    return approx * h;
}

void total_sum(int a, int b, int n, int* global_sum) {
    int thread_num = omp_get_thread_num();
    int num_threads = omp_get_num_threads();
    int h = (b-a) / n;
    int local_n = n / num_threads;
    int local_a = a + h * local_n * thread_num;
    int local_b = local_a + local_n * h;

    int local_sum = sum(local_a, local_b, local_n);
    printf("thread %d has found local sum as %d\n", thread_num, local_sum);
    

    #pragma omp critical
    *global_sum += local_sum;
}