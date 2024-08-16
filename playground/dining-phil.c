#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<semaphore.h>
#include<unistd.h>

sem_t table;
sem_t chopstick[5];

void * philosopher(void *);
void eat(int);
int main()
{
	int i;
	pthread_t tid[5];
	
	sem_init(&table,0,4);
	
	for(i=0;i<5;i++)
		sem_init(&chopstick[i],0,1);
		
	for(i=0;i<5;i++){
		int *num = malloc(sizeof(int));
        *num = i;
		pthread_create(&tid[i],NULL,philosopher, num);
	}
	for(i=0;i<5;i++)
		pthread_join(tid[i],NULL);
}

void * philosopher(void * num)
{
	int phil=*(int *)num;

	sem_wait(&table);
	printf("\nPhilosopher %d has entered table",phil);
	sem_wait(&chopstick[phil]);
	sem_wait(&chopstick[(phil+1)%5]);

	eat(phil);
	sleep(1);
	printf("\nPhilosopher %d has finished eating",phil);

	sem_post(&chopstick[(phil+1)%5]);
	sem_post(&chopstick[phil]);
	sem_post(&table);
}

void eat(int phil)
{
	printf("\nPhilosopher %d is eating",phil);
}