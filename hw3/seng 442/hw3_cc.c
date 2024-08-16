#include <stdio.h>
#include <mpi.h>

#define n 10

int a[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

int main(int argc, char* argv[])
{
    int pid, np;
    int elements_per_process;
    int local_a[n]; // Local array for each process
    int local_sum = 0, total_sum = 0;

    // Initialize MPI
    MPI_Init(&argc, &argv); //defines a communicator that consists of all the processes created when the program is started.

    // Get process ID and number of processes
    MPI_Comm_rank(MPI_COMM_WORLD, &pid); //the process making this call
    MPI_Comm_size(MPI_COMM_WORLD, &np); //number of processes in the communicator(a collection of proccesses that can send messages to each other)

    // Calculate the number of elements per process
    elements_per_process = n / np;

    // Scatter the elements of the array to all processes
    MPI_Scatter(a, elements_per_process, MPI_INT, local_a, elements_per_process, MPI_INT, 0, MPI_COMM_WORLD);

    // Each process calculates the sum of its local array
    for (int i = 0; i < elements_per_process; i++) {
        local_sum += local_a[i];
    }

    // Reduce all local sums to get the total sum in the root process
    MPI_Reduce(&local_sum, &total_sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD); //it sums the all partial arrays and returns it to the main process

    // Root process prints the final sum
    if (pid == 0) {
        printf("Sum of array is : %d\n", total_sum);
    }

    // Finalize MPI
    MPI_Finalize();

    return 0;
}
