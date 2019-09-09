// Nisa pinar ruzgar 220201050

#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>

#define NamedPipe1 "\\\\.\\pipe\\NamedPipeP1"
#define NamedPipe2 "\\\\.\\pipe\\NamedPipeP2"
#define NamedPipe3 "\\\\.\\pipe\\NamedPipeP3"
#define NamedPipe4 "\\\\.\\pipe\\NamedPipeP4"
#define NamedPipe5 "\\\\.\\pipe\\NamedPipeP5"

#define NO_OF_PROCESS 5
#define alpha 0.5


int main(int argc, char* argv[])
{
	int process_id;
	int executionNumber = 0;
	int burst = 0;
	int predictedBurst = 0, nextPredictedBurst = 0;
	

	//variables for pipes
	LPCSTR pipes[NO_OF_PROCESS] = { NamedPipe1, NamedPipe2, NamedPipe3, NamedPipe4, NamedPipe5 };
	HANDLE hPipe;
	BOOL flg;
	DWORD cbBytesRead, dwBytesWritten;

	if (argc != 2)
	{

		printf("\nID of child process is missing...\n");
		system("pause");
		exit(0);
	}

	// read the argument from parent process
	process_id = atoi(argv[1]);
	printf("\nHello from child process %d.\n ", process_id);

	//create a pipe
	hPipe = CreateFile(pipes[process_id - 1], GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL, NULL);

	if (hPipe == INVALID_HANDLE_VALUE)
	{
		printf("\nCreateFile failed for Named Pipe\n");
		exit(0);
	}
	while (executionNumber != 5)			            // Scheduling by Shortest Job First algorithm  --5 times
	{
		// read the pipe to get predicted burst 
		flg = ReadFile(hPipe, &predictedBurst, sizeof(int), &cbBytesRead, NULL);
		if (flg == FALSE)
		{
			printf("\nReadFile failed for Named Pipe\n");
			system("pause");
			exit(0);
		}
		
		printf("\n%d. execution:\n", executionNumber + 1);
		srand(time(NULL)*(executionNumber+1)*process_id);	    //to get a different seed at every execution   --normally child processes checks the clock in parallel
		burst = (rand() % 250) + 50;	                    //generates a random number in the range of 50 to 300
		printf("\nActual length of Nth CPU burst is %d \n", burst);
	
		nextPredictedBurst = alpha * burst + (1 - alpha) * predictedBurst;
		printf("\nPredicted burst is %d\n", predictedBurst);
		printf("\nNext predicted burst is %d \n", nextPredictedBurst);
		printf("\n \n");

		Sleep(burst);
	
		// write results to pipe
		flg = WriteFile(hPipe, &nextPredictedBurst, sizeof(int), &dwBytesWritten, NULL);
		if (flg == FALSE)
		{
			printf("\nWriteFile failed for Named Pipe , from child process %d\n\n", process_id);
			system("pause");
			exit(0);
		}
		executionNumber++;
	}	

	CloseHandle(hPipe);
	system("pause");
	return 1;
}
