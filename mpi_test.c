#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>

int main(int argc, char** argv) {
    int comm_size;
    int my_rank;

    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    int a = 1, b = 0, c = 2, d = 0;

    if(my_rank == 0 || my_rank == 2) {
        MPI_Reduce(&a, &b, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
        if(my_rank == 0) {
            printf("result of b after first reduce = %d \n", b);  // This should be 2
        }
    } else {
        MPI_Reduce(&c, &d, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
        if(my_rank == 0) {
            printf("result of d after first reduce = %d \n", d);  // This should be 4
        }
    }

    if(my_rank == 0 || my_rank == 2) {
        MPI_Reduce(&c, &d, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
        if(my_rank == 0) {
            printf("result of d after second reduce = %d \n", d);  // This should be 8
        }
    } else {
        MPI_Reduce(&a, &b, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
        if(my_rank == 0) {
            printf("result of b after second reduce = %d \n", b);  // This should be 4
        }
    }

    MPI_Recv()

    MPI_Finalize();
    return 0;
}
