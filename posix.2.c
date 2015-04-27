//posix version hw5.2	

#include <stdio.h>

#include <pthread.h>

#include <stdlib.h>

#include <math.h>

#include <semaphore.h>



#define BUFFERSIZE 16



int buffer[BUFFERSIZE];

int emptystart=0, fullstart=0;

pthread_mutex_t mutex;

sem_t full, empty;



typedef struct argue{

	int threadno, totalthreads, itemno;

} argue, *argues;



int consumerthread(void *ar);

int producerthread(void *ar);



int main(int argc, char *argv[]){

	pthread_t *handlethreads;

	int producers, consumers, items, pandc;

	argue *datapass;

	if(argc != 4 ){ // check for proper number of arguments

		printf("Too few arguments. (need 3)\n");

		return 1;

	}

	producers = pow(2, atoi(argv[1]));

	consumers = pow(2, atoi(argv[2]));

	items = pow(2, atoi(argv[3]));



	pandc = producers + consumers;

	handlethreads = malloc(pandc * sizeof(pthread_t));

	datapass = calloc(pandc, sizeof(argue));

	if(datapass == NULL){

		printf("error with calloc\n");

	} 



	printf("# of producers: %d, # of consumers, %d, # of items each producer makes: %d\n", producers, consumers, items);

	//create semaphores empty and full

	sem_init(&full, 0, 0);

	sem_init(&empty, 0, BUFFERSIZE);

	pthread_mutex_init(&mutex, NULL);



	// creates producer threads

	for(int i =0; i< producers; i++){ 

		datapass[i]. threadno = i;

		datapass[i]. totalthreads = producers;

		datapass[i]. itemno = items;



		pthread_create(&handlethreads[i], NULL, &producerthread, &datapass[i]);

	}

	

	// creates consumer threads

	for(int i =producers; i< pandc; i++){ 

		datapass[i]. threadno = i-producers+1;

		datapass[i]. totalthreads = consumers;

		datapass[i]. itemno = (items * producers) / consumers;

		pthread_create(&handlethreads[i], NULL, &consumerthread, &datapass[i]);

	}

	for(int i =0; i<pandc; i++){

		pthread_join(handlethreads[i], NULL);

	}

	printf("producers and consumers finished.\n");

	

	sem_destroy(&empty);

	sem_destroy(&full);

	pthread_mutex_destroy(&mutex);	

}



int producerthread(void *ar){

	argue *a = ar;

	int itemsproduced = 0;

	while(itemsproduced < a->itemno){

		sem_wait(&empty);

			pthread_mutex_lock(&mutex);

				buffer[fullstart] = (a->threadno * a->itemno) + itemsproduced;

				if(fullstart >= BUFFERSIZE){

					fullstart = 0;

				}

				else{

					fullstart++;

				}

			pthread_mutex_unlock(&mutex);		

		sem_post(&full);

		itemsproduced++;		

	}

	return 0;

}



int consumerthread(void *ar){

	argue *a = ar;

	int waitresult, itemsconsumed = 0;

	while(itemsconsumed< a->itemno){

		sem_wait(&full);

			pthread_mutex_lock(&mutex);

				printf("consumer %d ate %d\n",a->threadno, buffer[emptystart]);

				if(emptystart >= BUFFERSIZE){

					emptystart = 0;

				}

				else{

					emptystart++;

				}

			pthread_mutex_unlock(&mutex);	

		sem_post(&empty);

		itemsconsumed++;

	}

	return 0;

}