#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define ID 0
#define AGE 1 
#define LEFT 2
#define RIGHT 3

int rows = 100;
int num_targets = 2000;

typedef struct {
    int id;
    int age;
} BTree;

void locate(int index, int* node_array, BTree *node) {
    node_array[index * 4 + ID] = node->id;
    node_array[index * 4 + AGE] = node->age;
    node_array[index * 4 + LEFT] = -1;
    node_array[index * 4 + RIGHT] = -1;
}

void InsertNode(int root, BTree *node, int index, int *node_array) {
    if (node->age > node_array[root * 4 + AGE]) {
        if (node_array[root * 4 + RIGHT] == -1) {
            node_array[root * 4 + RIGHT] = index;
            locate(index, node_array, node);
        } else {
            InsertNode(node_array[root * 4 + RIGHT], node, index, node_array);
        }
    } else {
        if (node->age <= node_array[root * 4 + AGE]) {
            if (node_array[root * 4 + LEFT] == -1) {
                node_array[root * 4 + LEFT] = index;
                locate(index, node_array, node);
            } else {
                InsertNode(node_array[root * 4 + LEFT], node, index, node_array);
            }
        }
    }
}

void CreateTree(int size, int *node_array, int *target_values) {
    int i;
    BTree *tree = (BTree *)malloc(sizeof(BTree));
    tree->id = rand() % 10000;
    tree->age = rand() % 60 + 10;  // Initial random age
    locate(0, node_array, tree);
    for (i = 1; i < size; i++) {
        BTree *node = (BTree *)malloc(sizeof(BTree));
        node->id = rand() % size;
        node->age = rand() % 60 + 10;
        InsertNode(0, node, i, node_array);
    }

    // Insert target values into random nodes
    for (i = 0; i < num_targets; i++) {
        int target_node_index = rand() % size;
        node_array[target_node_index * 4 + AGE] = target_values[i];  // Ensure the target values are inserted
    }
}

void search(int *target_values, int num_targets, int *node_array, int index, int my_rank, double start_time, double *found_times) {
    if (index == -1) return;

    for (int i = 0; i < num_targets; i++) {
        if (found_times[i] == -1.0 && node_array[index * 4 + AGE] == target_values[i]) {
            found_times[i] = MPI_Wtime() - start_time;
        }
    }

    search(target_values, num_targets, node_array, node_array[index * 4 + LEFT], my_rank, start_time, found_times);
    search(target_values, num_targets, node_array, node_array[index * 4 + RIGHT], my_rank, start_time, found_times);
}

void distribute_work(int *node_array, int *work_list, int numtasks) {
    if (numtasks == 1) {
        work_list[0] = 0;
        return;
    }

    work_list[0] = 0; // Ensure all processes start from the root

    for (int i = 1; i < numtasks; i++) {
        work_list[i] = 0; // All processes start from the root
    }
}

int main(int argc, char **argv) {
    if (argc > 1) {
        rows = atoi(argv[1]);
    }

    int taskid, numtasks;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &taskid);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);

    int tree[rows][4]; // 0 :ID, 1 :age, 2 :left , 3 : right
    int target_values[num_targets];
    double found_times[num_targets];

    // Initialize target values
    for (int i = 0; i < num_targets; i++) {
        target_values[i] = rand() % 60 + 10;
        found_times[i] = -1.0; // Initialize found_times with invalid values
    }

    // The root process (taskid = 0) creates the binary tree
    if (taskid == 0) {
        CreateTree(rows, (int *)tree, target_values);
    }

    MPI_Barrier(MPI_COMM_WORLD); // Places a barrier to synchronize all the processes.
    MPI_Bcast(tree, rows * 4, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(target_values, num_targets, MPI_INT, 0, MPI_COMM_WORLD);

    // int* work_list = (int*)malloc(sizeof(int) * numtasks);
    int targets_per_process = num_targets / numtasks;
    int *local_targets = (int*)malloc(sizeof(int) * targets_per_process);
    double *local_found_times = (double*)malloc(sizeof(double) * targets_per_process);

    // if (taskid == 0) {
    //     distribute_work((int *)tree, work_list, numtasks);
    // }

    MPI_Scatter(target_values, targets_per_process, MPI_INT, local_targets, targets_per_process, MPI_INT, 0, MPI_COMM_WORLD);

    // int begin_node;
    // MPI_Scatter(work_list, 1, MPI_INT, &begin_node, 1, MPI_INT, 0, MPI_COMM_WORLD);

    double start_time = MPI_Wtime();
    search(local_targets, targets_per_process, (int *)tree, 0, taskid, start_time, local_found_times); // Start from root

    double end_time = MPI_Wtime();
    double search_time = end_time - start_time;

    MPI_Barrier(MPI_COMM_WORLD);

    double *global_found_times = NULL;
    if (taskid == 0) {
        global_found_times = (double*)malloc(sizeof(double) * num_targets);
    }

    // Gathers the found times of processes
    MPI_Gather(local_found_times, targets_per_process, MPI_DOUBLE, global_found_times, targets_per_process, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    double *search_times = NULL;
    if (taskid == 0) {
        search_times = (double*)malloc(sizeof(double) * numtasks);
    }

    MPI_Gather(&search_time, 1, MPI_DOUBLE, search_times, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    double max_search_time;
    MPI_Reduce(&search_time, &max_search_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

    if (taskid == 0) {
        // for (int i = 0; i < num_targets; i++) {
        //     double min_time = -1.0;
        //     int min_rank = -1;
        //     for (int j = 0; j < numtasks; j++) {
        //         if (global_found_times[i + j * targets_per_process] >= 0.0 && (min_time == -1.0 || global_found_times[i + j * targets_per_process] < min_time)) {
        //             min_time = global_found_times[i + j * targets_per_process];
        //             min_rank = j;
        //         }
        //     }

        //     if (min_rank != -1) {
        //         printf("Process %d found target value %d first in time %lf\n", min_rank, target_values[i], min_time);
        //     } else {
        //         printf("Target value %d not found by any process.\n", target_values[i]);
        //     }
        // }

        printf("Search times for all processes:\n");
        for (int i = 0; i < numtasks; i++) {
            printf("Process %d search time: %lf\n", i, search_times[i]);
        }
        printf("Total execution time %lf\n", max_search_time);

        free(global_found_times);
        free(search_times);
    }

    // free(work_list);
    free(local_targets);
    free(local_found_times);
    MPI_Finalize();
    return 0;
}
