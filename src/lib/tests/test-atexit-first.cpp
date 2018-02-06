/*****************************************************
             PROJECT  : MALT
             VERSION  : 1.0.0
             DATE     : 02/2018
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
#include <cstdio>
#include <cstdlib>

/**
 * This is a simple test case to valid usage. It can be used in all modes.
 *  - STACK_MODE='enter-exit' LD_PRELOAD=../src/libAllocStackProfiler.so
 *  - LD_PRELOAD=../src/libAllocStackProfiler.so
**/

/*******************  FUNCTION  *********************/
void funcAtExit(void)
{
	
}

/*******************  FUNCTION  *********************/
int main(void)
{
	atexit(funcAtExit);
	void * ptr = calloc(16,16);
	ptr = realloc(ptr,32*32);

	return 0;
}
