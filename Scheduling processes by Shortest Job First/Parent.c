// Nisa pinar ruzgar 220201050

#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <limits.h>

#define NamedPipe1 "\\\\.\\pipe\\NamedPipeP1"
#define NamedPipe2 "\\\\.\\pipe\\NamedPipeP2"
#define NamedPipe3 "\\\\.\\pipe\\NamedPipeP3"
#define NamedPipe4 "\\\\.\\pipe\\NamedPipeP4"
#define NamedPipe5 "\\\\.\\pipe\\NamedPipeP5"

#define NO_OF_PROCESS 5
#define BUFSIZE 1024

void findExecutionOrder(int predictedBurst[NO_OF_PROCESS], int executionOrder[NO_OF_PROCESS]);
void copyArray(int source[NO_OF_PROCESS], int target[NO_OF_PROCESS]);
void printExecutionOrder(int executionOrder[NO_OF_PROCESS], int executionNo);

int main(int argc, char* argv[])
{
	int i, x, pindex;
	int executionNumber = 0;        // for SRJ scheduling part

	int predictedBurst[5] = { 300, 220, 180, 45, 255 };
	int executionOrder[NO_OF_PROCESS];       // consist of indexes of processes starting from having the smallest CPU burst time to the biggest
	int copiedPredBurst[NO_OF_PROCESS];

	//variables for creating processes
	STARTUPINFO start_info[NO_OF_PROCESS];
	PROCESS_INFORMATION process_info[NO_OF_PROCESS];
	SECURITY_ATTRIBUTES security_att[NO_OF_PROCESS];
	HANDLE handles[NO_OF_PROCESS];
	char* lpCommandLine[NO_OF_PROCESS] = { "Child.exe 1", "Child.exe 2", "Child.exe 3", "Child.exe 4", "Child.exe 5"};


	//variables for creating named pipes
	BOOL fConnected;
	LPTSTR NamedPipes[NO_OF_PROCESS] = { NamedPipe1, NamedPipe2, NamedPipe3, NamedPipe4, NamedPipe5 };
	HANDLE hPipes[NO_OF_PROCESS];
	BOOL fSuccess;
	DWORD cbBytesRead;
	DWORD dwBytesWritten;
	BOOL flg;

	//creating NO_OF_PROCESS and connecting with its pipe
	for (i = 0; i < NO_OF_PROCESS; i++)
	{
		SecureZeroMemory(&start_info[i], sizeof(STARTUPINFO));
		start_info[i].cb = sizeof(STARTUPINFO);					// size of the startupinfo in bytes

		SecureZeroMemory(&process_info[i], sizeof(PROCESS_INFORMATION));

		SecureZeroMemory(&security_att[i], sizeof(SECURITY_ATTRIBUTES));
		security_att[i].bInheritHandle = TRUE;
		security_att[i].lpSecurityDescriptor = NULL;
		security_att[i].nLength = sizeof(SECURITY_ATTRIBUTES);

		if (!CreateProcess(NULL, lpCommandLine[i], NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &start_info[i], &process_info[i]))
		{
			printf("Unable to create process %d: %s", i, GetLastError());
			system("pause");
			exit(0);
		}
		else
		{
			handles[i] = process_info[i].hProcess;
		}

		// PIPE
		//create a named pipe for each process
		hPipes[i] = CreateNamedPipe(NamedPipes[i], PIPE_ACCESS_DUPLEX, PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT, 1,
			BUFSIZE, BUFSIZE, NMPWAIT_USE_DEFAULT_WAIT, NULL);

		if (hPipes[i] == INVALID_HANDLE_VALUE) {
			printf("\nCreating pipe %d failed", i);
			system("pause");
			exit(0);
		}
		
		// Wait for the child process to connect; if it succeeds, 
		// the function returns a nonzero value. If the function returns 
		// zero, GetLastError returns ERROR_PIPE_CONNECTED

		fConnected = ConnectNamedPipe(hPipes[i], NULL) ?
			TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);
		
		if (!fConnected)
		{	// The child process could not connect in the CreateNamedPipe sample, so close the pipe. 
			CloseHandle(hPipes[i]);
			system("pause");
			exit(0);
		}

	}

	
	while (executionNumber != 5)			 // Scheduling by Shortest Job First algorithm --5 times
	{
		copyArray(predictedBurst, copiedPredBurst);
		findExecutionOrder(copiedPredBurst, executionOrder);
		printExecutionOrder(executionOrder, executionNumber);

		for (x = 0; x < NO_OF_PROCESS; x++)		// executing processes in SJF order
		{
			//write predictedBurst to each child process
			pindex = executionOrder[x];
			printf("P%d started working.\n", pindex + 1);
			
			fSuccess = WriteFile(hPipes[pindex], &predictedBurst[pindex], sizeof(int), &dwBytesWritten, NULL);

			if (fSuccess == FALSE)
			{
				printf("\nWriteFile failed for Named Pipe %d\n", pindex + 1);
				system("pause");
				exit(0);
			}

			// read the pipe to get next predicted burst and set it as the new predicted burst
			flg = ReadFile(hPipes[pindex], &predictedBurst[pindex], sizeof(int), &cbBytesRead, NULL);
			if (flg == FALSE)
			{
				printf("\nReadFile failed for Named Pipe %d\n", pindex + 1);
				system("pause");
				exit(0);
			}
			else
			{
				printf("P%d ended working.\n", pindex + 1);
			}
			
		}
		printf("\n  \n");
		executionNumber++;
	}


	WaitForMultipleObjects(NO_OF_PROCESS, handles, TRUE, INFINITE);

	//close all handles
	for (i = 0; i<NO_OF_PROCESS; i++)
	{

		CloseHandle(process_info[i].hProcess);
		CloseHandle(process_info[i].hThread);
	}
	return 1;
}


void findExecutionOrder(int predictedBurst[NO_OF_PROCESS], int executionOrder[NO_OF_PROCESS])  
{
	int x = 0;        
	int i, indexOfCurrentMin=0;
	double min;
	while (x != NO_OF_PROCESS)		// looping through executionOrder
	{
		min = INT_MAX;                          //to guarantee always entering the for loop
		for (i = 0; i < NO_OF_PROCESS; i++)     //looping through predictedBurst
		{
			if (predictedBurst[i] < min)
			{
				min = predictedBurst[i];
				indexOfCurrentMin = i;
			}
		}
		predictedBurst[indexOfCurrentMin] = INT_MAX;         //for not to be selected again
		executionOrder[x] = indexOfCurrentMin;
		x++;
	}
}


void copyArray(int source[NO_OF_PROCESS], int target[NO_OF_PROCESS])
{
	int i;
	for (i = 0; i < NO_OF_PROCESS; i++)
	{
		target[i] = source[i];
	}

}

void printExecutionOrder(int executionOrder[NO_OF_PROCESS], int executionNo)
{
	int i;
	printf("\n%d. Execution Order <", executionNo + 1);
	for (i = 0; i < NO_OF_PROCESS; i++) 
	{
		if (i == NO_OF_PROCESS-1) 
		{
			printf("P%d>\n", executionOrder[i] + 1);
		}
		else
		{
			printf("P%d, ", executionOrder[i] + 1);
		}
	}

}