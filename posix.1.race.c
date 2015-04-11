#include <unistd.h>
#include <string.h>
//posix version hw4
#include <fcntl.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#define threadnum 8
#define bytes 65536

char buffer[bytes];
int countarray[128];

typedef struct argue{
	int buffstart;
	int buffend;
} argue, *argues;

int ascicount(void *ar);

int main(int argc, char *argv[]){

	int *openfile, readcount, threadbyte, n;
	argue *datapass;
	pthread_t handlethreads[threadnum];
	pthread_attr_t thread;
	readcount = 0;

	if(access(argv[1], F_OK) == 0) { // check that source file exists
		
		openfile = open(argv[1], O_RDONLY);
	}
	else{
		printf("Source file does not exist.\n");
		return 1;
	}	

	readcount = read(openfile, &buffer, bytes);
	threadbyte = readcount/threadnum;
	n = pthread_attr_init(&thread);
	if(n !=0){	
			printf("an error occurred\n");
			return 1;
		}

	datapass = calloc(threadnum, sizeof(argue));
	if(datapass == NULL){
		printf("error with calloc\n");
	} 
	for(int i =0; i< threadnum; i++){
		
		datapass[i]. buffstart = i * threadbyte;

		if(i==threadnum-1){
			datapass[i]. buffend = (i * threadbyte) + threadbyte + threadnum;
		}
		else {
			datapass[i]. buffend = (i * threadbyte) + threadbyte;
		}
		
		pthread_create(&handlethreads[i], &thread, &ascicount, &datapass[i]);
	}
	
	for(int i =0; i<threadnum; i++){
		pthread_join(handlethreads[i], NULL);
		printf("test\n");
		
	}

	for(int i = 0; i<128;i++){
		if(i<33 || i>126){
			printf("%d occurences of 0x%x\n", countarray[i], i);
		}
		else {
			printf("%d occurences of %c\n", countarray[i], i);
		}
	}
	pthread_exit(0);
}

int ascicount(void *ar){
	argue *a = ar;
	for(int i= a->buffstart; i< (a->buffend);i++){
		countarray[buffer[i]]++;
	}
	return 0;
}