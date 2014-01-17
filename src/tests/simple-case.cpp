/********************  HEADERS  *********************/
#include <cstdlib>
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
	free(malloc(32));
	funcC();
}

/*******************  FUNCTION  *********************/
void funcA()
{
	free(malloc(16));
	funcB();
}

/*******************  FUNCTION  *********************/
void recurseA(int depth)
{
	free(malloc(64));
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
		free(ptr1);
		free(ptr2);
	}
}

/*******************  FUNCTION  *********************/
int main(void)
{
	//first is calloc
	free(calloc(16,16));
	
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

	return 0;
}
