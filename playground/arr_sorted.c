#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#define NUM_THREADS 2
#define ARR_SIZE 10

int arr[10] = {1,2,3,4,5,6,7,8,123,10};
int isSorted = 1;

void* arr_sorted(void* args);

int main() {
    pthread_t th[NUM_THREADS];

    for(int i = 0; i < NUM_THREADS; i++ ) {
        int* rank = (malloc(sizeof(int)));
        *rank = i;
        pthread_create(&th[i], NULL, arr_sorted, rank);
        rank++;
    }
    for(int i = 0; i < NUM_THREADS; i++ ) {
        pthread_join(th[i], NULL);
    }

    if(isSorted != 0) {
        printf("The array is sorted\n");
        return 0;
    }

    printf("The array is not sorted\n");
    return 0;    
}

void* arr_sorted(void* args) {
    int rank = *((int*)(args));
    printf("the rank: %d\n", rank);
    int start_index = rank * ARR_SIZE / NUM_THREADS;
    int end_index = start_index + ARR_SIZE / NUM_THREADS;

    for(int i = start_index; i < end_index; i++) {
        for(int j = i; j < ARR_SIZE; j++) {
            if(arr[i] < arr[j]) {
                isSorted = 0;
                pthread_exit(NULL);
            }
        }
    }
}