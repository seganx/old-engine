#include <stdio.h>
#include <process.h>
#include <windows.h>

static HANDLE my_semaphore = CreateSemaphore(NULL, 0, 1, NULL);
static int * my_counter = NULL;

void worker(void * object)
{
	int* threadId = (int*)object;
	//while (true)
	{
		my_counter[1]++;

		//WaitForSingleObject(my_semaphore, INFINITE);

		for (int i = 0; i < (1000000 + rand()); ++i)
			*my_counter += rand();

		printf("\r%.3d : %d", *threadId, *my_counter);

		my_counter[1]--;
	}
}



int main(char* arg[])
{
	printf("create threads...\n");
	my_counter = (int*)calloc(4, 4);
	my_counter[1] = 0;

	printf("address: %p\n", my_counter);

	for (int i=0; i<500; ++i)
	{
		int* index = (int*)calloc(1, 4);
		*index = i;

		_beginthread(worker, 0, index);
	}

#if 0
	char c = 0;
	while ( c != 'q' )
	{
		ReleaseSemaphore(my_semaphore, 1, NULL);
		c = getchar();
	}
#endif

	Sleep(10);
	while (my_counter[1] > 0)
		Sleep(1);

	printf("\naddress: %p\n", my_counter);
	free(my_counter);

	getchar();

	return 0;
}

