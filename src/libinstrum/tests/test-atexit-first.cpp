/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.6
*    DATE     : 09/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/tests/test-atexit-first.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2014 - 2024
*    AUTHOR   : Sébastien Valat (ECR) - 2014
***********************************************************/

/**********************************************************/
#include <cstdio>
#include <cstdlib>

/**
 * This is a simple test case to valid usage. It can be used in all modes.
 *  - STACK_MODE='enter-exit' LD_PRELOAD=../src/libAllocStackProfiler.so
 *  - LD_PRELOAD=../src/libAllocStackProfiler.so
**/

/**********************************************************/
void funcAtExit(void)
{
	
}

/**********************************************************/
int main(void)
{
	atexit(funcAtExit);
	void * ptr = calloc(16,16);
	ptr = realloc(ptr,32*32);

	return 0;
}
