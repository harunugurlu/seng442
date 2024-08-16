#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

#define MAX_VERTICES 10000  // Maximum number of vertices
#define MAX_WEIGHT 100      // Maximum weight of edges

// Function to find the minimum distance vertex
int minDistance(int *dist, int *sptSet, int num_vertices) {
    int min = INT_MAX, min_index = -1;
    #pragma omp parallel for reduction(min:min)
    for (int v = 0; v < num_vertices; v++) {
        if (!sptSet[v] && dist[v] < min) {
            #pragma omp critical
            {
                if (dist[v] < min) {
                    min = dist[v];
                    min_index = v;
                }
            }
        }
    }
    return min_index;
}

// Function to implement Dijkstra's algorithm
void dijkstra(int *graph, int src, int num_vertices) {
    int *dist = malloc(num_vertices * sizeof(int));
    int *sptSet = malloc(num_vertices * sizeof(int));

    #pragma omp parallel for
    for (int i = 0; i < num_vertices; i++) {
        dist[i] = INT_MAX;
        sptSet[i] = 0;
    }

    dist[src] = 0;

    for (int count = 0; count < num_vertices - 1; count++) {
        int u = minDistance(dist, sptSet, num_vertices);
        sptSet[u] = 1;

        #pragma omp parallel for
        for (int v = 0; v < num_vertices; v++) {
            if (!sptSet[v] && *(graph + u * num_vertices + v) && dist[u] != INT_MAX && dist[u] + *(graph + u * num_vertices + v) < dist[v]) {
                dist[v] = dist[u] + *(graph + u * num_vertices + v);
            }
        }
    }

    free(dist);
    free(sptSet);
}

// Function to generate a random graph
void generateRandomGraph(int *graph, int num_vertices) {
    srand(time(NULL)); // Seed the random number generator

    for (int i = 0; i < num_vertices; i++) {
        for (int j = 0; j < num_vertices; j++) {
            if (i != j) {
                int randWeight = rand() % MAX_WEIGHT + 1; // Random weight from 1 to MAX_WEIGHT
                *(graph + i * num_vertices + j) = (rand() % 100 < 50) ? randWeight : 0; // 50% chance there is an edge
            } else {
                *(graph + i * num_vertices + j) = 0; // No self-loops
            }
        }
    }
}

int main(int argc, char *argv[]) {
    int num_vertices = (argc > 1) ? atoi(argv[1]) : 5; // Default or command line input
    int num_threads = (argc > 2) ? atoi(argv[2]) : 1; // Default or command line input

    int *graph = malloc(num_vertices * num_vertices * sizeof(int));
    generateRandomGraph(graph, num_vertices); // Generate a random graph

    omp_set_num_threads(num_threads);

    double start_time = omp_get_wtime();
    dijkstra(graph, 0, num_vertices);
    double end_time = omp_get_wtime();

    printf("Time taken for Dijkstra's algorithm with %d vertices and %d threads: %f seconds\n", num_vertices, num_threads, end_time - start_time);

    free(graph);
    return 0;
}
