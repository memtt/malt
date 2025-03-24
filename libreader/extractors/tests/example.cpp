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

/**********************************************************/
void leaf(void)
{
	void * ptr1 = malloc(16);
	void * ptr2 = malloc(1024*1024);
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
int main(int argc, char * argv[])
{
	caller();
	return EXIT_SUCCESS;
}
