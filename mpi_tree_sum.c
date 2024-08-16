#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>

int get_pow(int n);

int main(int argc, char** argv) {
    int comm_size;
    int my_rank;

    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    // int arr[] = {5, 2, -1, -3, 6, 5, -7, 2};
    int arr[] = {5, 2, -1, -3};

    int local_sum = arr[my_rank];

    int total_pass = get_pow(comm_size);
    int curr_pass = 0;

    // if(my_rank % 2 != 0) {
    //     MPI_Send(&local_sum, 1, MPI_INT, my_rank-1, 0, MPI_COMM_WORLD);
    // }
    
    // if()

    for(int i = 0; i < total_pass; i++) {
        if(i % 2 != 0) {
            MPI_Send(&local_sum, 1, MPI_INT, my_rank-1, 0, MPI_COMM_WORLD);
        }
    }

    MPI_Finalize();
    return 0;
}

int get_pow(int n) {
    int count = 0;
    while(n % 2 == 0) {
        n /= 2;
        count++;
    }
    return count;
}