#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

void CreateTree(int size, int * node_array)
{
    int i;
    BTree * tree = (BTree *)malloc(sizeof(BTree));
    tree->id = rand() % 10000;
    tree->age = rand() % 40 + 10;
    locate(0, node_array, tree);
    for (i = 1; i < size; i++)
    {
        BTree * node = (BTree *)malloc(sizeof(BTree));
        node->id = rand() % size;
        node->age = rand() % 60 + 10;
        InsertNode(0, node, i, node_array);
    }
}

void search(int age, int * node_array, int index)
{
    if (node_array[index * 4 + AGE] == age)
    {
        printf("ID = %d, Age= %d\n", node_array[index * 4 + ID], node_array[index * 4 + AGE]);
    }
    if (node_array[index * 4 + LEFT] != -1)
    {
        search(age, node_array, node_array[index * 4 + LEFT]);
    }
    if (node_array[index * 4 + RIGHT] != -1)
    {
        search(age, node_array, node_array[index * 4 + RIGHT]);
    }
}

void check(int targetAge, int * node_array, int index)
{
    if (node_array[index * 4 + AGE] == targetAge)
    {
        printf("ID = %d, Age= %d\n", node_array[index * 4 + ID], node_array[index * 4 + AGE]);
    }
}

void get_more_work(int * work_list, int * numTasks, int numProcesses, int targetAge, int * node_array)
{
    // If the number of assigned tasks is equal to the number of processes, return.
    if ((*numTasks) == numProcesses)
    {
        return;
    }
    else
    {
        (*numTasks)--;
        int root = work_list[*numTasks];
        check(targetAge, node_array, root);
        int left = node_array[root * 4 + LEFT];
        int right = node_array[root * 4 + RIGHT];  // Corrected from LEFT to RIGHT
        if (left != -1)
        {
            work_list[*numTasks] = left;
            (*numTasks)++;
        }
        if (right != -1)
        {
            work_list[*numTasks] = right;
            (*numTasks)++;
        }
        get_more_work(work_list, numTasks, numProcesses, targetAge, node_array);
    }
}

int main(int argc, char **argv)
{
    rows = atoi(argv[1]);

    int taskid, numtasks;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &taskid);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);

    int tree[rows][4]; // 0 :ID, 1 :age, 2 :left , 3 : right

    // The root process (taskid = 0) creates the binary tree
    if (taskid == 0)
    {
        CreateTree(rows, (int *)tree);
    }

    int tragetAGE = 30;

    MPI_Barrier(MPI_COMM_WORLD); // Places a barrier to synchronize all the processes.
    MPI_Bcast(tree, rows * 4, MPI_INT, 0, MPI_COMM_WORLD);
    
    double time;
    time = -MPI_Wtime();
    int* work_list = (int*)malloc(sizeof(int) * numtasks);

    if (taskid == 0)
    {
        int work = 1; // how many tasks have been assigned so far
        work_list[0] = 0;
        get_more_work(work_list, &work, numtasks, tragetAGE, (int *)tree);
    }

    int begin_node;
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Scatter(work_list, 1, MPI_INT, &begin_node, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);
    search(40, (int *)tree, begin_node);
    MPI_Barrier(MPI_COMM_WORLD);
    time += MPI_Wtime();

    if (taskid == 0)
        printf("Time:%lf, numtasks:%d\n", time, numtasks);
    free(work_list);
    MPI_Finalize();
    return 0;
}
