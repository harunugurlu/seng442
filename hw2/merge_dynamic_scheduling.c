#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
//static dynamic scheduling
void merge(int arr[], int l, int m, int r) {
    int n1 = m - l + 1;
    int n2 = r - m;
    int L[n1], R[n2];

    // Dynamically schedule the copying of elements to temporary arrays
    #pragma omp parallel for schedule(dynamic)
    for (int i = 0; i < n1; i++)
        L[i] = arr[l + i];

    #pragma omp parallel for schedule(dynamic)
    for (int j = 0; j < n2; j++)
        R[j] = arr[m + 1 + j];

    int i = 0, j = 0, k = l;
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {
            arr[k] = L[i];
            i++;
        } else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }

    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }

    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }
}



void mergeSort(int arr[], int l, int r) {
    if (l < r) {
    int m = l + (r - l) / 2;

    #pragma omp parallel
    {
        #pragma omp single nowait
        {
            #pragma omp task
            mergeSort(arr, l, m);

            #pragma omp task
            mergeSort(arr, m + 1, r);
            
            #pragma omp taskwait
        }
    }
    merge(arr, l, m, r);
}

}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <array_size> <num_threads>\n", argv[0]);
        return 1;
    }

    int arr_size = atoi(argv[1]); // Convert command-line argument to integer
    int num_threads = atoi(argv[2]); // Convert command-line argument to integer
    omp_set_num_threads(num_threads);

    int arr[arr_size];
    
    // Initialize the array with random values for demonstration
    srand(123); // Seed for reproducibility
    for (int i = 0; i < arr_size; i++) {
        arr[i] = rand() % 10000; // Random values between 0 and 99
    }

    // printf("Given array is \n");
    // for (int i = 0; i < arr_size; i++)
    //     printf("%d ", arr[i]);

    const double tstart = omp_get_wtime();
    mergeSort(arr, 0, arr_size - 1);
    const double elapsed = omp_get_wtime() - tstart;

    // printf("\nSorted array is \n");
    // for (int i = 0; i < arr_size; i++)
    //     printf("%d ", arr[i]);
    // printf("\n");

    printf("Elapsed time: %f\n", elapsed);
    
    return 0;
}
