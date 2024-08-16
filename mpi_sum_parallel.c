#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>

int* create_arr(int comm_size);
int sum(int* arr);

int main(int argc, char** argv) {
    int comm_size;
    int my_rank;

    srand(time(NULL));

    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    int *arr;
    if(my_rank == 0) {
        arr = create_arr(comm_size);
    }

    int arr_size = 20;
    printf("arr_size is %d\n", arr_size);
    MPI_Barrier(MPI_COMM_WORLD);

    MPI_Bcast(&arr_size, 1, MPI_INT, 0, MPI_COMM_WORLD);

    for(int i = 0; i < 5; i++) {

    }

    int *rec_arr = malloc(arr_size / comm_size);

    MPI_Scatter(arr, (arr_size / comm_size), MPI_INT, rec_arr, (arr_size / comm_size), MPI_INT, 0, MPI_COMM_WORLD);

    int local_sum = sum(rec_arr);
    int total_sum = 0;

    MPI_Reduce(&local_sum, &total_sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if(my_rank == 0) {
        printf("Total sum is %d\n", total_sum);
    }

    MPI_Finalize();
    return 0;
}

int* create_arr(int comm_size) {
    int *arr = malloc(sizeof(int) * comm_size*5);
    for(int i = 0; i < comm_size*5; i++) {
        arr[i] = rand() % 100;
        printf("random number arr[%d] : %d\n", i, arr[i]);
    }
    return arr;
}

int sum(int* arr) {
    int result = 0;
    int n = 5;
    for(int i = 0; i < n; i++) {
        result += arr[i];
    }
    return result;
}