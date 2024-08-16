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

typedef struct
{
    int id;
    int age;
} BTree;

void locate(int index, int* node_array, BTree * node)
{
    node_array[index * 4 + ID] = node->id;
    node_array[index * 4 + AGE] = node->age;
    node_array[index * 4 + LEFT] = -1;
    node_array[index * 4 + RIGHT] = -1;
}

void InsertNode(int root, BTree * node, int index, int * node_array)
{
    if (node->age > node_array[root * 4 + AGE])
    {
        if (node_array[root * 4 + RIGHT] == -1)
        {
            node_array[root * 4 + RIGHT] = index;
            locate(index, node_array, node);
        }
        else
        {
            InsertNode(node_array[root * 4 + RIGHT], node, index, node_array);
        }
    }    
    else
    {
        if (node->age <= node_array[root * 4 + AGE])
        {
            if (node_array[root * 4 + LEFT] == -1)
            {
                node_array[root * 4 + LEFT] = index;
                locate(index, node_array, node);
            }
            else
            {
                InsertNode(node_array[root * 4 + LEFT], node, index, node_array);
            }
        }
    }
}

void CreateTree(int size, int * node_array, int target_age)
{
    int i;
    BTree * tree = (BTree *)malloc(sizeof(BTree));
    tree->id = rand() % 10000;
    tree->age = target_age;  // Ensure the target value is inserted
    locate(0, node_array, tree);
    for (i = 1; i < size; i++)
    {
        BTree * node = (BTree *)malloc(sizeof(BTree));
        node->id = rand() % size;
        node->age = rand() % 60 + 10;
        InsertNode(0, node, i, node_array);
    }
}

int search(int age, int * node_array, int index, int my_rank, double start_time, double *found_time)
{
    if (index == -1) return 0;

    if (node_array[index * 4 + AGE] == age)
    {
        *found_time = MPI_Wtime() - start_time;
        return 1;
    }
    
    if (age < node_array[index * 4 + AGE])
    {
        return search(age, node_array, node_array[index * 4 + LEFT], my_rank, start_time, found_time);
    }
    else
    {
        return search(age, node_array, node_array[index * 4 + RIGHT], my_rank, start_time, found_time);
    }
}

void bfs_distribute(int *node_array, int *work_list, int numtasks)
{
    int queue[rows];
    int front = 0, rear = 0;
    int work_count = 0;

    queue[rear++] = 0; // Start from the root

    while (front < rear && work_count < numtasks) {
        int node = queue[front++];
        work_list[work_count++] = node;

        int left = node_array[node * 4 + LEFT];
        int right = node_array[node * 4 + RIGHT];

        if (left != -1) queue[rear++] = left;
        if (right != -1) queue[rear++] = right;
    }

    // If fewer nodes than processes, fill remaining work list with -1
    for (int i = work_count; i < numtasks; i++) {
        work_list[i] = -1;
    }
}

int main(int argc, char **argv)
{
    if (argc > 1) {
        rows = atoi(argv[1]);
    }

    int taskid, numtasks;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &taskid);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);

    int tree[rows][4]; // 0 :ID, 1 :age, 2 :left , 3 : right

    // The root process (taskid = 0) creates the binary tree
    if (taskid == 0)
    {
        CreateTree(rows, (int *)tree, 30);
    }

    int targetAGE = 30;

    MPI_Barrier(MPI_COMM_WORLD); // Places a barrier to synchronize all the processes.
    MPI_Bcast(tree, rows * 4, MPI_INT, 0, MPI_COMM_WORLD);
    
    double start_time = MPI_Wtime();
    int* work_list = (int*)malloc(sizeof(int) * numtasks);

    if (taskid == 0)
    {
        bfs_distribute((int *)tree, work_list, numtasks);
    }

    int begin_node;
    MPI_Scatter(work_list, 1, MPI_INT, &begin_node, 1, MPI_INT, 0, MPI_COMM_WORLD);

    double found_time = -1.0; // Initialize found_time with an invalid value
    search(targetAGE, (int *)tree, begin_node, taskid, start_time, &found_time);

    MPI_Barrier(MPI_COMM_WORLD);

    double global_times[numtasks];
    MPI_Gather(&found_time, 1, MPI_DOUBLE, global_times, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    if (taskid == 0)
    {
        double min_time = -1.0;
        int min_rank = -1;
        for (int i = 0; i < numtasks; i++)
        {
            if (global_times[i] >= 0.0 && (min_time == -1.0 || global_times[i] < min_time))
            {
                min_time = global_times[i];
                min_rank = i;
            }
        }

        if (min_rank != -1)
        {
            printf("Process %d found the target first in time %lf\n", min_rank, min_time);
        }
        else
        {
            printf("Target not found by any process.\n");
        }
    }

    free(work_list);
    MPI_Finalize();
    return 0;
}
