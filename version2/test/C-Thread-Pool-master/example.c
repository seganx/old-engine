/* 
 * WHAT THIS EXAMPLE DOES
 * 
 * We create a pool of 4 threads and then add 40 tasks to the pool(20 task1 
 * functions and 20 task2 functions). task1 and task2 simply print which thread is running them.
 * 
 * As soon as we add the tasks to the pool, the threads will run them. It can happen that 
 * you see a single thread running all the tasks (highly unlikely). It is up the OS to
 * decide which thread will run what. So it is not an error of the thread pool but rather
 * a decision of the OS.
 * 
 * */

#include <stdio.h>
#if defined(_WIN32)
#include <windows.h>
#include <process.h>
#else
#include <pthread.h>
#endif
#include "thpool.h"


void task1(){
#if defined(_WIN32)
	int id = (int)GetThreadId(GetCurrentThread());
#else
	int id = (int)pthread_self();
#endif
	printf("Thread #%u working on task1\n", id);
}


void task2(){
#if defined(_WIN32)
	int id = (int)GetThreadId(GetCurrentThread());
#else
	int id = (int)pthread_self();
#endif
	printf("Thread #%u working on task2\n", id);
}


int main(){
	
	puts("Making threadpool with 100 threads\n");
	threadpool thpool = thpool_init(100);
	getchar();

	puts("Adding 40 tasks to threadpool\n");
	int i;
	for (i=0; i<20; i++){
		thpool_add_work(thpool, (void*)task1, NULL);
		thpool_add_work(thpool, (void*)task2, NULL);
	};

	getchar();

	puts("Killing threadpool");
	thpool_destroy(thpool);
	
	puts("Finished");
	getchar();

	return 0;
}
