//win32 version hw4
#include <windows.h>
#include <stdio.h>
#include <tchar.h>

#define threadnum 4
#define bytes 65536

char buffer[bytes];
int countarray[threadnum][128];

typedef struct argue{
	int buffstart;
	int buffend;
	int arrayno;
} argue, *argues;

DWORD WINAPI ascicount(void *ar);

int main(int argc, char *argv[]){
	
	int *openfile, readcount, threadbyte;
	argues datapass[threadnum];
	HANDLE handlethreads[threadnum];
	int finalcount[128] ={0};
	readcount = 0;

	openfile = CreateFile(argv[1], GENERIC_READ, 0, NULL, 3, FILE_ATTRIBUTE_NORMAL, NULL);
	if(openfile == INVALID_HANDLE_VALUE){ // check that source file exists
		printf("Source file does not exist.\n");
		return 1;
	}

	ReadFile(openfile, &buffer, bytes, &readcount, NULL);
	threadbyte = readcount/ threadnum;

	for(int i =0; i< threadnum; i++){
		datapass[i] = (argues) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(argue));
		datapass[i]-> buffstart = i * threadbyte;
		datapass[i]-> arrayno = i;
		if(i==threadnum-1){
			datapass[i]-> buffend = (i * threadbyte) + threadbyte + threadnum;
		}
		else {
			datapass[i]-> buffend = (i * threadbyte) + threadbyte;
		}
		handlethreads[i] = CreateThread(NULL, 0, ascicount, datapass[i], 0, NULL);
	}
	
	WaitForMultipleObjects(threadnum, handlethreads, TRUE, INFINITE);

	for(int i =0; i<threadnum;i++){
		CloseHandle(handlethreads[i]);
	}
	
	for(int i =0; i<128;i++){
		for(int j =0; j < threadnum;j++){
			finalcount[i] += countarray[j][i];
		}		
	}
	for(int i = 0; i<128;i++){
		if(i<33 || i>126){
			printf("%d occurences of 0x%x\n", finalcount[i], i);
		}
		else {
			printf("%d occurences of %c\n", finalcount[i], i);
		}
	}
}

DWORD WINAPI ascicount(LPVOID ar){
	argue *a = ar;
	for(int i= a->buffstart; i< (a->buffend);i++){
		countarray[a->arrayno][buffer[i]]++;
	}
	return 0;
}