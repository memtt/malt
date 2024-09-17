/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.2
*    DATE     : 11/2014
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/tests/test-fragmentation.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (ECR) - 2014
***********************************************************/
/********************  HEADERS  *********************/
#include <stdlib.h>
#include <unistd.h>

/********************  MACROS  **********************/
#define COUNT 4

/*******************  FUNCTION  *********************/
int main(void)
{
	void * ptr[COUNT];
	for (int i = 0 ; i < COUNT ; i++)
		ptr[i] = malloc(16);
	sleep(1);
	for (int i = 0 ; i < COUNT ; i+=2)
		free(ptr[i]);
	sleep(1);
	for (int i = 1 ; i < COUNT ; i+=2)
		free(ptr[i]);
	return EXIT_SUCCESS;
}