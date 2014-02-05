/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <iostream>

/**
 * This is a simple test case to valid usage. It can be used in all modes.
 *  - STACK_MODE='enter-exit' LD_PRELOAD=../src/libAllocStackProfiler.so
 *  - LD_PRELOAD=../src/libAllocStackProfiler.so
**/

/*******************  FUNCTION  *********************/
void funcC()
{
	int * ptr = new int[16];
	delete [] ptr;
}

/*******************  FUNCTION  *********************/
void funcB()
{
	void * ptr = malloc(32);
	*(char*)ptr='c';//required otherwise new compilers will remove malloc/free
	free(ptr);
	funcC();
}

/*******************  FUNCTION  *********************/
void funcA()
{
	void * ptr = malloc(16);
	*(char*)ptr='c';//required otherwise new compilers will remove malloc/free
	free(ptr);
	funcB();
}

/*******************  FUNCTION  *********************/
void recurseA(int depth)
{
	void * ptr = malloc(64);
	*(char*)ptr='c';//required otherwise new compilers will remove malloc/free
	free(ptr);
	if (depth > 0)
		recurseA(depth-1);
}

/*******************  FUNCTION  *********************/
void testRealloc(void)
{
	void * ptr = realloc(NULL,16);
	ptr = realloc(ptr,32);
	ptr = realloc(ptr,0);
	
	free(malloc(44));
}

/*******************  FUNCTION  *********************/
void testMaxAlive(void)
{
	for (int i = 0 ; i < 100 ; i++)
	{
		void * ptr1 = malloc(64);
		void * ptr2 = malloc(64);
		*(char*)ptr1='c';//required otherwise new compilers will remove malloc/free
		*(char*)ptr2='c';//required otherwise new compilers will remove malloc/free
		free(ptr1);
		free(ptr2);
	}
}

/*******************  FUNCTION  *********************/
void testThreads(void)
{
	#pragma omp parallel for
	for (int i = 0 ; i < 10000 ; i++)
	{
		void * ptr = malloc(64);
		*(char*)ptr='c';
		free(ptr);
	}
}

/*******************  FUNCTION  *********************/
int main(void)
{
	//first is calloc
	void * ptr = calloc(16,16);
	*(char*)ptr='c';//required otherwise new compilers will remove malloc/free
	free(ptr);
	
	funcA();
	for (int i = 0 ; i < 10 ; ++i)
	{
		funcB();
	}
	recurseA(10);
	for (int i = 0 ; i < 10 ; ++i)
	{
		recurseA(15);
	}
	testRealloc();
	testMaxAlive();
	testThreads();

	return 0;
}
