#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <omp.h>

void merge(int array[], int low, int mid, int high)
{
    int temp[1000];
    int i, j, k, m;
    j = low;
    m = mid + 1;
    for (i = low; j <= mid && m <= high; i++)
    {
        if (array[j] <= array[m])
        {
            temp[i] = array[j];
            j++;
        }
        else
        {
            temp[i] = array[m];
            m++;
        }
    }
    if (j > mid)
    {
        for (k = m; k <= high; k++)
        {
            temp[i] = array[k];
            i++;
        }
    }
    else
    {
        for (k = j; k <= mid; k++)
        {
            temp[i] = array[k];
            i++;
        }
    }
    for (k = low; k <= high; k++)
        array[k] = temp[k];
}

void mergesort(int array[], int low, int high)
{
    int mid;
    if (low < high)
    {
        mid = (low + high) / 2;

#pragma omp parallel sections num_threads(2)
        {
#pragma omp section
            {
                mergesort(array, low, mid);
            }

#pragma omp section
            {
                mergesort(array, mid + 1, high);
            }
        }
        merge(array, low, mid, high);
    }
}

/* Returns a random integer in the range [a..b], inclusive */
int randab(int a, int b)
{
    return a + rand() % (b-a+1);
}

void swap(int* a, int* b)
{
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

void fill(int* a, int n)
{
    int i;
    for (i=0; i<n; i++) {
        a[i] = (int)i;
    }
    for (i=0; i<n-1; i++) {
        int j = randab(i, n-1);
        swap(a+i, a+j);
    }
}

int main( int argc, char* argv[] )
{
    int n = 10000000;

    if ( argc > 2 ) {
        fprintf(stderr, "Usage: %s [n]\n", argv[0]);
        return EXIT_FAILURE;
    }

    if ( argc > 1 ) {
        n = atoi(argv[1]);
    }

    if (n > 1000000000) {
        fprintf(stderr, "FATAL: array too large\n");
        return EXIT_FAILURE;
    }

    int *a = (int*)malloc(n*sizeof(a[0]));
    assert(a != NULL);

    printf("Initializing array...\n");
    fill(a, n);
    printf("Sorting %d elements...", n); fflush(stdout);
    mergesort(a, 0, n - 1);
    printf("Sorted Elements as follows:\n");
    for (int i = 0; i < n; i++)
        printf("%d ", a[i]);
    printf("\n");
    return 0;
}