//win32 version hw4
#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <math.h>

#define BUFFERSIZE 16

int buffer[BUFFERSIZE];
int emptystart=0, fullstart=0;
HANDLE full, empty, mutex;

typedef struct argue{
	int threadno, totalthreads, itemno;
} argue, *argues;

DWORD WINAPI consumerthread(LPVOID *ar);
DWORD WINAPI producerthread(LPVOID *ar);

int main(int argc, char *argv[]){
	HANDLE *handlethreads;
	int producers, consumers, items, pandc;
	argues *datapass;
	if(argc != 4 ){ // check for proper number of arguments
		printf("Too few arguments. (need 3)\n");
		return 1;
	}
	producers = pow(2, atoi(argv[1]));
	consumers = pow(2, atoi(argv[2]));
	items = pow(2, atoi(argv[3]));
	pandc = producers + consumers;
	handlethreads = malloc(pandc * sizeof(HANDLE));
	datapass = malloc(pandc * sizeof(HANDLE));

	printf("# of producers: %d, # of consumers, %d, # of items each producer makes: %d\n", producers, consumers, items);
	//create semaphores empty and full
	full = CreateSemaphore(NULL, 0, BUFFERSIZE, NULL);
	empty = CreateSemaphore(NULL, BUFFERSIZE, BUFFERSIZE, NULL);
	mutex = CreateMutex(NULL, FALSE, NULL);

	 if (full == NULL|| empty == NULL|| mutex == NULL){    
        printf("Error: %d\n", GetLastError());
        return 1;
    }

	// creates producer threads
	for(int i =0; i< producers; i++){ 
		datapass[i] = (argues) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(argue));
		datapass[i]-> threadno = i;
		datapass[i]-> totalthreads = producers;
		datapass[i]-> itemno = items;
		handlethreads[i] = CreateThread(NULL, 0, producerthread, datapass[i], 0, NULL);
	}

	// creates consumer threads
	for(int i =producers; i< pandc; i++){ 
		datapass[i] = (argues) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(argue));
		datapass[i]-> threadno = i-producers+1;
		datapass[i]-> totalthreads = consumers;
		datapass[i]-> itemno = (items * producers) / consumers;
		handlethreads[i] = CreateThread(NULL, 0, consumerthread, datapass[i], 0, NULL);
	}
	
	WaitForMultipleObjects(pandc, handlethreads, TRUE, INFINITE);

	for(int i =0; i<pandc;i++){
		CloseHandle(handlethreads[i]);
	}
	CloseHandle(empty);
	CloseHandle(full);
	CloseHandle(mutex);
	printf("producers and consumers finished.\n");
}

DWORD WINAPI producerthread(LPVOID ar){
	argue *a = ar;
	int waitresult, itemsproduced = 0;
	while(itemsproduced < a->itemno){
		waitresult = WaitForSingleObject(empty, INFINITE);
		if(waitresult == WAIT_OBJECT_0){
			waitresult = WaitForSingleObject(mutex, INFINITE);
			if(waitresult == WAIT_OBJECT_0){
				buffer[fullstart] = (a->threadno * a->itemno) + itemsproduced;
				if(fullstart >= BUFFERSIZE){
					fullstart = 0;
				}
				else{
					fullstart++;
				}
				ReleaseMutex(mutex);	
				}
			}		
		ReleaseSemaphore(full, 1, NULL);
		itemsproduced++;		
	}
	return 0;
}

DWORD WINAPI consumerthread(LPVOID ar){
	argue *a = ar;
	int waitresult, itemsconsumed = 0;
	while(itemsconsumed< a->itemno){
		waitresult = WaitForSingleObject(full, INFINITE);
		if(waitresult == WAIT_OBJECT_0){
			waitresult = WaitForSingleObject(mutex, INFINITE);
			if(waitresult == WAIT_OBJECT_0){
				printf("consumer %d ate %d\n",a->threadno, buffer[emptystart]);
				if(emptystart >= BUFFERSIZE){
					emptystart = 0;
				}
				else{
					emptystart++;
				}
				ReleaseMutex(mutex);	
				}
			}		
		ReleaseSemaphore(empty, 1, NULL);
		itemsconsumed++;
	}
	return 0;
}