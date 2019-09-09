
// Nisa pinar ruzgar 220201050

#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#define NamedPipe1 "\\\\.\\pipe\\NamedPipeMon"
#define NamedPipe2 "\\\\.\\pipe\\NamedPipeTue"
#define NamedPipe3 "\\\\.\\pipe\\NamedPipeWed"
#define NamedPipe4 "\\\\.\\pipe\\NamedPipeThr"
#define NamedPipe5 "\\\\.\\pipe\\NamedPipeFri"
#define NamedPipe6 "\\\\.\\pipe\\NamedPipeSat"
#define NamedPipe7 "\\\\.\\pipe\\NamedPipeSun"

#define NO_OF_PROCESS 7
#define NO_OF_PRODUCT 4
#define BUFSIZE 1024

int main(int argc, char* argv[])
{
	int i, j, pid = 0;
	int max_index = 0;
	int x, count = 0, max = 0;

	int indexes[NO_OF_PRODUCT];
	int data[NO_OF_PROCESS][NO_OF_PRODUCT];

	//varibles for creating processes
	STARTUPINFO start_info[NO_OF_PROCESS];
	PROCESS_INFORMATION process_info[NO_OF_PROCESS];
	SECURITY_ATTRIBUTES security_att[NO_OF_PROCESS];
	HANDLE handles[NO_OF_PROCESS];

	char* lpCommandLine[NO_OF_PROCESS] = { "client.exe 1", "client.exe 2", "client.exe 3", "client.exe 4", "client.exe 5", "client.exe 6", "client.exe 7" };

	//variables for creating named pipes
	BOOL fConnected;
	LPTSTR NamedPipes[NO_OF_PROCESS] = { NamedPipe1, NamedPipe2, NamedPipe3, NamedPipe4, NamedPipe5, NamedPipe6, NamedPipe7 };
	HANDLE hPipes[NO_OF_PROCESS];

	BOOL fSuccess;
	DWORD cbBytesRead;
	DWORD dwBytesWritten;

	//creating NO_OF_PROCESS and connecting with its pipe
	for (i = 0; i<NO_OF_PROCESS; i++)
	{
		SecureZeroMemory(&start_info[i], sizeof(STARTUPINFO));
		start_info[i].cb = sizeof(STARTUPINFO);						//the size of the startupinfo in bytes

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

		if (fConnected)
		{
			pid = i + 1;

			//write number of day to each child process
			fSuccess = WriteFile(hPipes[i], &pid, sizeof(int) + 1, &dwBytesWritten, NULL);

			if (fSuccess == FALSE)
			{
				printf("\nWriteFile failed for Named Pipe %d\n", i + 1);
				system("pause");
				exit(0);
			}
			else
			{
				printf("\n\nNumber of day sent as %d to child process %d ... \n ", pid, pid);
			}

			//read the data written by child process
			fSuccess = ReadFile(hPipes[i], data[i], NO_OF_PRODUCT * sizeof(int), &cbBytesRead, NULL);

			if (fSuccess == FALSE)
			{
				printf("\nReadFile failed for Named Pipe %d\n", i + 1);
				system("pause");
				exit(0);
			}
			else
			{
				printf("\nReadFile succeeded for Named Pipe %d \n ", i + 1);
			}

		}
		else
		{	// The child process could not connect in the CreateNamedPipe sample, so close the pipe. 
			CloseHandle(hPipes[i]);
			system("pause");
			exit(0);
		}

	}

	//all results gathered, print resuts
	printf("\n\n 1) The total number of each item sold in each day");
	for (i = 0; i < NO_OF_PROCESS; i++)
	{
		printf("\n\n .... Day %d .... ", i + 1);

		printf("\n\n TOTAL MILK PRODUCT: %d ", data[i][0]);
		printf("\n TOTAL BISCUIT PRODUCT: %d ", data[i][1]);
		printf("\n TOTAL CHIPS PRODUCT: %d ", data[i][2]);
		printf("\n TOTAL COKE PRODUCT: %d \n", data[i][3]);
	}

	printf("\n 2)The total number of each item sold in seven days");

	for (j = 0; j < NO_OF_PRODUCT; j++)
	{
		count = 0;
		max = 0;

		for (i = 0; i < NO_OF_PROCESS; i++) {
			count = count + data[i][j];
		}

		if (max < count)
		{
			max = count;
			max_index = j;
		}

		if (j == 0)
			printf("\n\n TOTAL MILK PRODUCT OF SEVEN DAYS: %d ", count);

		else if (j == 1)
			printf("\n TOTAL BISCUIT PRODUCT OF SEVEN DAYS: %d ", count);

		else if (j == 2)
			printf("\n TOTAL CHIPS PRODUCT OF SEVEN DAYS: %d ", count);

		else
			printf("\n TOTAL COKE PRODUCT OF SEVEN DAYS: %d \n", count);

	}

	printf("\n\n 3)The most sold item for each day\n");

	for (i = 0; i < NO_OF_PROCESS; i++)
	{
		max = 0;

		for (j = 0; j < NO_OF_PRODUCT; j++)
		{
			if (max < data[i][j])
				max = data[i][j];
		}

		x = 0;
		for (j = 0; j < NO_OF_PRODUCT; j++)
		{
			if (data[i][j] == max)
				indexes[x] = j;
			else
				indexes[x] = 300;
			x++;
		}

		printf("\n Day %d:  ", i + 1);
		x = 0;
		for (x = 0; x < NO_OF_PRODUCT; x++)
		{
			if (indexes[x] == 0)
				printf("MILK ");
			else if (indexes[x] == 1)
				printf("BISCUITS ");
			else if (indexes[x] == 2)
				printf("CHIPS ");
			else if (indexes[x] == 3)
				printf("COKE ");

		}

	}

	printf("\n\n 4)The most sold item in seven days: ");

	if (max_index == 0)
		printf("MILK");
	else if (max_index == 1)
		printf("BISCUIT");
	else if (max_index == 2)
		printf("CHIPS");
	else
		printf("COKE");

	WaitForMultipleObjects(NO_OF_PROCESS, handles, TRUE, INFINITE);

	//close all handles
	for (i = 0; i<NO_OF_PROCESS; i++)
	{
		FlushFileBuffers(hPipes[i]);
		DisconnectNamedPipe(hPipes[i]);
		CloseHandle(hPipes[i]);

		CloseHandle(process_info[i].hProcess);
		CloseHandle(process_info[i].hThread);
	}
	return 0;
}

