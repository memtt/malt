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
int main(void)
{
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
}
