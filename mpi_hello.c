#include <stdio.h>
#include <mpi.h>
#include <string.h>

const int MAX_STRING = 100;

int main(int argc, char **argv) {

    char greeting[MAX_STRING];
    int comm_size;
    int my_rank;

    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    printf("test my_rank: %d\n", my_rank);

    if(my_rank != 0) {
        sprintf(greeting, "This is process %d", my_rank);
        MPI_Send(greeting, strlen(greeting), MPI_CHAR, 0, 0, MPI_COMM_WORLD);
    }
    else {
        for(int i = 1; i < comm_size; i++) {
            MPI_Recv(greeting, MAX_STRING, MPI_CHAR, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            printf("Message from process %d: %s\n", i, greeting);
        }
    }

    MPI_Finalize();
    return 0;
}