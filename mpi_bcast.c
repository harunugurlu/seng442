#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <string.h>

void bcast(void* data, int count, int my_rank, int comm_size, MPI_Datatype DATATYPE, int root, MPI_Comm communicator) {

    if(my_rank == 0) {
        for(int i = 1; i < comm_size; i++) {
            MPI_Send(data, count, DATATYPE, i, 0, communicator);
        }
    }
    else {
        MPI_Recv(data, count, DATATYPE, root, 0, communicator, MPI_STATUS_IGNORE);
    }
}

int main(int argc, char** argv) {

    int comm_size;
    int my_rank;
    
    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    int data = 23;

    char message[] = "naberasdasdasdasd";

    bcast(message, strlen(message)+1, my_rank, comm_size, MPI_CHAR, 0, MPI_COMM_WORLD);

    printf("Process %d got message as %s\n", my_rank, message);

    MPI_Finalize();
    return 0;
}

