#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>

void do_work(int *sum, int *my_array);

int main(int argc, char** argv) {
    
    int comm_size;
    int my_rank;

    int arr[100];

    srand(0);

    int i;
    for (i = 0; i < 100; i++) {
        arr[i] = i;
    }

    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    
    int my_index = 0;
    int my_array[25];
    int my_sum = 0;
    int total_sum = 0;
    MPI_Scatter(arr, 25, MPI_INT, my_array, 25, MPI_INT, 0, MPI_COMM_WORLD);

    do_work(&my_sum, my_array);

    MPI_Reduce(&my_sum, &total_sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if(my_rank == 0) {
        printf("total sum is %d\n", total_sum);
    }
    
    MPI_Finalize();
    return 0;
}

void do_work(int *sum, int *my_array) {
    for(int i = 0; i < 25; i++) {
        *sum += my_array[i];
    }
}