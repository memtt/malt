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
#include <malloc.h>
#include <iostream>
#include <cassert>

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
	if (depth > 0)
	{
		void * ptr = malloc(64);
		*(char*)ptr='c';//required otherwise new compilers will remove malloc/free
		free(ptr);
		recurseA(depth-1);
	}
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
	for (int i = 0 ; i < 100 ; i++)
	{
		void * ptr = malloc(64);
		*(char*)ptr='c';
		free(ptr);
	}
}

/*******************  FUNCTION  *********************/
void testRecuseIntervedB(int depth);
void testRecuseIntervedA(int depth)
{
	void * ptr = malloc(64);
	*(char*)ptr='c';
	free(ptr);
	testRecuseIntervedB(depth);
}

/*******************  FUNCTION  *********************/
void testRecuseIntervedB(int depth)
{
	void * ptr = malloc(64);
	*(char*)ptr='c';
	free(ptr);
	if (depth > 0)
		testRecuseIntervedA(depth-1);
}

/*******************  FUNCTION  *********************/
void testAllFuncs(void)
{
	void * ptr;
	
	//malloc
	ptr = malloc(16);
	*(char*)ptr = 'c';
	free(ptr);
	
	//calloc
	ptr = calloc(1,16);
	*(char*)ptr = 'c';
	free(ptr);
	
	//realloc
	ptr = realloc(NULL,16);
	*(char*)ptr = 'c';
	free(ptr);
	
	//posix_memalign
	int res = posix_memalign(&ptr,16,16);
	*(int*)ptr = res;
	free(ptr);
	
	//posix_memalign
	ptr = aligned_alloc(16,16);
	*(int*)ptr = res;
	free(ptr);
	
	//posix_memalign
	ptr = memalign(16,16);
	*(int*)ptr = res;
	free(ptr);
	
	//posix_memalign
	ptr = valloc(16);
	assert(ptr != NULL);
	*(int*)ptr = res;
	free(ptr);
	
	//posix_memalign
	ptr = pvalloc(16);
	assert(ptr != NULL);
	*(int*)ptr = res;
	free(ptr);
}

/*******************  FUNCTION  *********************/
void testZero(void)
{
	void * ptr = malloc(0);
	free(NULL);
	ptr = realloc(NULL,0);
	*(char*)ptr = 'c';
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
		recurseA(10);
	}
	testRealloc();
	testMaxAlive();
	testRecuseIntervedA(2);
	testThreads();
	testAllFuncs();

	return 0;
}
