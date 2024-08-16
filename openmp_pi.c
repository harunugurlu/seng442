#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

double pi_serial(int n);

int main(int argc, char** argv) {
    int thread_num = atoi(argv[1]);
    int n = 100;
    double factor = 1.0;
    double sum = 0.0;
    double pi_approx = 0.0;
    #pragma omp parallel for schedule(auto) num_threads(thread_num) \
    reduction(+:sum) private(factor)
        for(int k = 0; k < n; k++) {
            if(k % 2 == 0) factor = 1.0;
            else factor = -1.0;
            sum += factor / (2 * k + 1);
        }
    pi_approx = 4.0 * sum;
    

    printf("pi_approx = %f\n", pi_approx);
}

double pi_serial(int n) {
    double factor = 1.0;
    double sum = 0.0;
    for(int i = 0; i < n; i++) {
        sum += factor/(2*i+1);
        factor = -factor;
    }
    return 4.0 * sum;
}