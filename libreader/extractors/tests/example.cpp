/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 10/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/core/tests/TestAllocTraceFile.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2022 - 2024
***********************************************************/

/**********************************************************/
#include <cstdlib>
#include <cstring>
#include "state/malt.h"

/**********************************************************/
int maltInitStatus(void)
{
	return 0;
}

/**********************************************************/
void leaf(void)
{
	void * ptr1 = malloc(16);
	void * ptr2 = malloc(1024*1024);
	memset(ptr1, 0, 16);
	memset(ptr2, 0, 1024*1024);
	free(ptr1);
	free(ptr2);
}

/**********************************************************/
void callerRecurse(int depth)
{
	if (depth == 0)
		leaf();
	else
		callerRecurse(depth - 1);
}

/**********************************************************/
void caller(void)
{
	callerRecurse(3);
}

/**********************************************************/
void caller2(void)
{
	void * ptr1 = malloc(8);
	free(ptr1);
}

/**********************************************************/
void main_sim(void)
{
	caller();
	caller2();
}

/**********************************************************/
int main(int argc, char * argv[])
{
	maltEnable();	
	main_sim();
	maltDisable();
	return EXIT_SUCCESS;
}
