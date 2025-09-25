/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 06/2014
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/tests/test-atexit-first.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2014
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
