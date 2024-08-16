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

int search(int age, int * node_array, int index, int my_rank, double start_time, int numtasks)
{
    if (index == -1) return 0;

    if (node_array[index * 4 + AGE] == age)
    {
        double time = MPI_Wtime() - start_time;
        printf("Process %d has found the target element ID = %d, Age = %d\n", my_rank, node_array[index * 4 + ID], node_array[index * 4 + AGE]);
        printf("Time: %lf, numtasks: %d\n", time, numtasks);
        MPI_Abort(MPI_COMM_WORLD, 0);
    
        return 1;
    }
    
    if (age < node_array[index * 4 + AGE])
    {
        return search(age, node_array, node_array[index * 4 + LEFT], my_rank, start_time, numtasks);
    }
    else
    {
        return search(age, node_array, node_array[index * 4 + RIGHT], my_rank, start_time, numtasks);
    }
}

void distribute_work(int *node_array, int *work_list, int numtasks)
{
    if (numtasks == 1) {
        work_list[0] = 0;
        return;
    }

    work_list[0] = node_array[0 * 4 + LEFT];
    work_list[1] = node_array[0 * 4 + RIGHT];

    int level = 1;
    int count = 2;
    int current = 0;

    while (count < numtasks && level < rows) {
        for (int i = 0; i < (1 << level) && count < numtasks; i++) {
            int index = (current + i) % count;
            int left_child = node_array[work_list[index] * 4 + LEFT];
            int right_child = node_array[work_list[index] * 4 + RIGHT];

            if (left_child != -1 && count < numtasks) {
                work_list[count++] = left_child;
            }
            if (right_child != -1 && count < numtasks) {
                work_list[count++] = right_child;
            }
        }
        level++;
        current += 1 << (level - 1);
    }

    // Fill remaining work list with -1 if fewer nodes than processes
    for (int i = count; i < numtasks; i++) {
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
        // Print the tree for debugging
        // printf("Tree created with %d nodes. Target age 30 guaranteed to be in the tree.\n", rows);
        // for (int i = 0; i < rows; i++) {
        //     printf("Node %d: ID=%d, Age=%d, Left=%d, Right=%d\n", i, tree[i][ID], tree[i][AGE], tree[i][LEFT], tree[i][RIGHT]);
        // }
    }

    int targetAGE = 30;

    MPI_Barrier(MPI_COMM_WORLD); // Places a barrier to synchronize all the processes.
    MPI_Bcast(tree, rows * 4, MPI_INT, 0, MPI_COMM_WORLD);
    
    double start_time = MPI_Wtime();
    int* work_list = (int*)malloc(sizeof(int) * numtasks);

    if (taskid == 0)
    {
        distribute_work((int *)tree, work_list, numtasks);
        // Print work distribution for debugging
        // printf("Work distribution:\n");
        // for (int i = 0; i < numtasks; i++) {
        //     printf("Process %d starts with node %d\n", i, work_list[i]);
        // }
    }

    int begin_node;
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Scatter(work_list, 1, MPI_INT, &begin_node, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);

    search(targetAGE, (int *)tree, begin_node, taskid, start_time, numtasks);

    // Synchronize and finalize properly
    MPI_Barrier(MPI_COMM_WORLD);
    double end_time = MPI_Wtime();

    if (taskid == 0)
        printf("Time: %lf, numtasks: %d\n", end_time - start_time, numtasks);

    free(work_list);
    MPI_Finalize();
    return 0;
}