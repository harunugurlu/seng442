#include <stdio.h>
#include <mpi.h>
#include <string.h>

const int MAX_STRING = 100;

int main() {
    char greeting[MAX_STRING];
    int comm_size;
    int my_rank;

    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Status status;

    if(my_rank != 0) {
        sprintf(greeting, "Greetings from process %d of %d!\n", my_rank, comm_size);
        MPI_Send(greeting, strlen(greeting)+1, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
    } else {
        printf("Greetings from process %d of %d!\n", my_rank, comm_size);
        for(int i = 1; i < comm_size; i++) {
            MPI_Recv(greeting, MAX_STRING, MPI_CHAR, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
            int sender_rank = status.MPI_SOURCE;
            int received_tag = status.MPI_TAG;
            int num_of_elements;
            MPI_Get_count(&status, MPI_CHAR, &num_of_elements);
            printf("Received %d characters from process %d with tag %d: %s\n", num_of_elements, sender_rank, received_tag, greeting);
            // printf("%s\n", greeting);
        }
    }

    MPI_Finalize();
    return 0;
}