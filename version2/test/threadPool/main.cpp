#include <stdio.h>
#include <process.h>
#include <windows.h>

static HANDLE my_semaphore = CreateSemaphore(NULL, 0, 1, NULL);

void worker(void * object)
{
	int* threadId = (int*)object;
	while (true)
	{
		WaitForSingleObject(my_semaphore, INFINITE);

		for (int i = 0; i < 10; ++i)
		{
			printf("%d ", *threadId);
			Sleep(1000);
		}
	}
}



int main(char* arg[])
{
	printf("create threads...");
	
	for (int i=0; i<5; ++i)
	{
		int* index = (int*)calloc(1, 4);
		*index = i;

		_beginthread(worker, 0, index);
	}

	while (true)
	{
		getchar();
		ReleaseSemaphore(my_semaphore, 1, NULL);
	}

	getchar();
	return 0;
}

