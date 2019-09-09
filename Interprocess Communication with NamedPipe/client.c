
// Nisa pinar ruzgar 220201050
#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <malloc.h>
#include <errno.h>

#define NamedPipe1 "\\\\.\\pipe\\NamedPipeMon"
#define NamedPipe2 "\\\\.\\pipe\\NamedPipeTue"
#define NamedPipe3 "\\\\.\\pipe\\NamedPipeWed"
#define NamedPipe4 "\\\\.\\pipe\\NamedPipeThr"
#define NamedPipe5 "\\\\.\\pipe\\NamedPipeFri"
#define NamedPipe6 "\\\\.\\pipe\\NamedPipeSat"
#define NamedPipe7 "\\\\.\\pipe\\NamedPipeSun"

#define NO_OF_PROCESS 7
#define NO_OF_THREAD 4

typedef struct myparam
{
	int tid;    //thread id
	int pid;    //process id
}param;

productCount[NO_OF_THREAD];

DWORD WINAPI threadwork(LPVOID  threadID);

int main(int argc, char* argv[])
{
	LPCSTR pipes[NO_OF_PROCESS] = { NamedPipe1, NamedPipe2, NamedPipe3, NamedPipe4, NamedPipe5, NamedPipe6, NamedPipe7 };
	HANDLE hPipe;
	BOOL flg;
	DWORD cbBytesRead, dwBytesWritten;
	int input = 0;

	int i, process_id;
	param *p[NO_OF_THREAD];
	HANDLE handles[NO_OF_THREAD];
	int threadID[NO_OF_THREAD];

	if (argc != 2)
	{
		printf("\nID of child process is missing...\n");
		system("pause");
		exit(0);
	}

	// read the argument from parent process
	process_id = atoi(argv[1]);
	printf("\nHello from child process %d, ", process_id);

	//create a pipe
	hPipe = CreateFile(pipes[process_id - 1], GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL, NULL);

	if (hPipe == INVALID_HANDLE_VALUE)
	{
		printf("\nCreateFile failed for Named Pipe\n");
	}
	else
	{
		// read the pipe to get which day to calculate
		flg = ReadFile(hPipe, &input, sizeof(int), &cbBytesRead, NULL);
		if (flg == FALSE)
		{
			printf("\nReadFile failed for Named Pipe\n");
			system("pause");
			exit(0);
		}
		else
		{
			printf("\nReadFile succeeded for Named Pipe\n");
		}
	}

	printf("\nProcess %d received day number as %d from parent process", process_id, input);

	//create threads and send arguments as thread id and process id
	for (i = 0; i<NO_OF_THREAD; i++)
	{
		productCount[i] = 0;                     //initialize product count as ZERO
		p[i] = (param*)malloc(sizeof(param));
		p[i]->tid = i + 1;
		p[i]->pid = process_id;

		//Create threads here
		handles[i] = CreateThread(NULL, 0, threadwork, p[i], 0, &threadID[i]);

		if (handles[i] == INVALID_HANDLE_VALUE)
		{
			printf("error in child process %d: cannot create thread %d\n", process_id, i);
			system("pause");
			exit(0);
		}

		printf("\n Thread %d has started working ...\n", p[i]->tid);
	}

	WaitForMultipleObjects(NO_OF_THREAD, handles, TRUE, INFINITE);

	printf("\n\n TOTAL MILK PRODUCT: %d ", productCount[0]);
	printf("\n TOTAL BISCUIT PRODUCT: %d ", productCount[1]);
	printf("\n TOTAL CHIPS PRODUCT: %d ", productCount[2]);
	printf("\n TOTAL COKE PRODUCT: %d \n", productCount[3]);


	// write results to pipe
	flg = WriteFile(hPipe, productCount, NO_OF_THREAD * sizeof(int), &dwBytesWritten, NULL);
	if (flg == FALSE)
	{
		printf("\nWriteFile failed for Named Pipe\n");
		system("pause");
		exit(0);
	}
	else
	{
		printf("\nWriteFile succeeded for Named Pipe\n");
	}

	for (i = 0; i<NO_OF_THREAD; i++)
	{
		CloseHandle(handles[i]);
	}

	CloseHandle(hPipe);
	system("pause");
	return 1;
}



DWORD WINAPI threadwork(LPVOID threadID)
{
	char threadProduct[10] = "";     //coke, biscuit, milk or chips
	char startingLine[128] = "";
	char endingLine[128] = "";

	char line[128];  			//  maximum line size 
	char *token, *next_token = NULL;
	int i, tid, pid;

	FILE *fPointer;
	errno_t err;

	param* p = (param*)threadID;    // read thread parameters
	tid = p->tid;
	pid = p->pid;

	printf("\nHello from thread %d of process %d \n", tid, pid);

	switch (tid)   //find product using id of the thread 
	{
	case 1:
		strcpy_s(threadProduct, 5, "MILK");
		break;
	case 2:
		strcpy_s(threadProduct, 8, "BISCUIT");
		break;
	case 3:
		strcpy_s(threadProduct, 6, "CHIPS");
		break;
	case 4:
		strcpy_s(threadProduct, 5, "COKE");
		break;

	}

	switch (pid) //find day using id of the child process
	{
	case 1:
		strcpy_s(startingLine, 15, "#START DAY 1#\n");
		strcpy_s(endingLine, 13, "#END DAY 1#\n");
		break;
	case 2:
		strcpy_s(startingLine, 15, "#START DAY 2#\n");
		strcpy_s(endingLine, 13, "#END DAY 2#\n");
		break;
	case 3:
		strcpy_s(startingLine, 15, "#START DAY 3#\n");
		strcpy_s(endingLine, 13, "#END DAY 3#\n");
		break;
	case 4:
		strcpy_s(startingLine, 15, "#START DAY 4#\n");
		strcpy_s(endingLine, 13, "#END DAY 4#\n");
		break;
	case 5:
		strcpy_s(startingLine, 15, "#START DAY 5#\n");
		strcpy_s(endingLine, 13, "#END DAY 5#\n");
		break;
	case 6:
		strcpy_s(startingLine, 15, "#START DAY 6#\n");
		strcpy_s(endingLine, 13, "#END DAY 6#\n");
		break;
	case 7:
		strcpy_s(startingLine, 15, "#START DAY 7#\n");
		strcpy_s(endingLine, 13, "#END DAY 7#");
		break;

	}

	//	printf("\n my product:%s, start line:%s, end line:%s", threadProduct, startingLine, endingLine);

	err = fopen_s(&fPointer, "market.txt", "r");

	if (err == 0)
	{
		// printf("\nThe file 'market.txt' is opened\n");

		while (fgets(line, sizeof(line), fPointer) != NULL) 	//read a line 
		{

			if (strcmp(line, startingLine) == 0)    //find the starting day
			{
				while (strcmp(line, endingLine) != 0)    //loop till the end of the day
				{
					fgets(line, sizeof(line), fPointer);

					if (strcmp(line, endingLine) == 0)   // exit if end of the day is reached
						break;

					token = strtok_s(line, ", ", &next_token);
					while (token != NULL)
					{
						//printf("\n a token found:%s of size:%d", token,sizeof(threadProduct));
						if (strncmp(token, threadProduct, 4) == 0)
						{
							productCount[tid - 1] ++;
							printf("\n ***** thread %d found a %s", tid, threadProduct);

						}

						token = strtok_s(NULL, ", ", &next_token);
					}

				}

				break;    // our day processed, exit loop 

			}

		}

		printf("\nThread %d found total %d %s product \n", tid, productCount[tid - 1], threadProduct);
	}

	else
	{
		printf("\nThe file 'market.txt' is not opened\n");
		exit(0);
	}

	fclose(fPointer);    //close the file

	printf("\n Thread %d finished working ...", tid);

	return 1;
}

