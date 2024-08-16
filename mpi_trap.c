#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>


double calculate_serial(double a, double b, int n);
void calculate_parallel(int a, int b, int n);
double f(double x);

int main(int argc, char** argv) {
    int comm_size;
    int my_rank;

    srand(time(NULL));

    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    int n = 20;
    int a = 0, b = 20;
    int h = (b - a) / n;

    double my_n = n / comm_size;
    double my_a = a + my_rank * my_n * h;
    double my_b = my_a + my_n * h;

    double my_result = calculate_serial(my_a, my_b, my_n);
    
    if(my_rank != 0) {
        MPI_Send(&my_result, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    }
    else {
        double rec_area = 0.0;
        printf("process 0 has calculated the area as %lf\n", my_result);
        double total_area = my_result;
        for(int i = 1; i < comm_size; i++) {
            MPI_Recv(&rec_area, 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            printf("process %d has sent the area %lf\n", i, rec_area);
            total_area += rec_area;
        }
        printf("The total area is %lf\n", total_area);
    }

    MPI_Finalize();
    return 0;
}

double f(double x) {
    return x*x;
}

double calculate_serial(double a, double b, int n) {
    double h = (b-a)/n;
    double approx = (f(a) + f(b)) / 2.0;
    for(int i = 1; i < n-1; i++) {
        double x_i = a + h*i;
        approx += f(x_i);
    }
    approx = h*approx;

    return approx;
}