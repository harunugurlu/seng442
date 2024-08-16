#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>

int area(int a, int b, int n);
int f(int x);

int main(int argc, char** argv) {
    int comm_size;
    int my_rank;

    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    int a = 2, b = 22;
    int h = (b-a)/2;

    int n = 5;

    int local_n = n / comm_size;
    int local_a = a + my_rank * local_n * h;
    int local_b = local_a + local_n * h;

    int local_area = area(local_a, local_b, local_n);

    // if(my_rank != 0) {
    //     MPI_Send(&local_area, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    // }
    // else {
    //     int total_area = local_area;
    //     int rec_area = 0;
    //     printf("Root process found area as %d\n", local_area);
    //     for(int i = 1; i < comm_size; i++) {
    //         MPI_Recv(&rec_area, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    //         total_area += rec_area;
    //         printf("Process %d sent its area as %d\n", i, rec_area);
    //     }
    //     printf("total area %d\n", total_area);
    // }
    int total_area = 0;
    int total_area2 = 0;
    MPI_Reduce(&local_area, &total_area, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    printf("total area %d\n", total_area);

    MPI_Barrier(MPI_COMM_WORLD);
    sleep(1);
    MPI_Allreduce(&local_area, &total_area2, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
    printf("total area2 %d\n", total_area2);

    MPI_Finalize();
    return 0;
}

int f(int x) {
    return x*x;
}

int area(int a, int b, int n) {
    int h = (b-a) / 2;
    int approx = (f(a) + f(b)) / 2;
    for(int i = 1; i <= n-1; i++) {
        int x_i = a + i*h;
        approx += f(x_i);
    }
    return approx * h;
}