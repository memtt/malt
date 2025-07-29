/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.3.1
*    DATE     : 02/2025
*    LICENSE  : CeCILL-C
*    FILE     : examples/cpp/step-10-problem-leak.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <list>

/**********************************************************/
void do_allocs_and_forget_one_free(void)
{
	//var
	void * buffer[4096];

	//alloc 4K arrays
	for (size_t i = 0 ; i < 4096 ; i++)
		buffer[i] = malloc(1024);

	//free by forgetting two of them...
	for (size_t i = 0 ; i < 4096 - 2 ; i++)
		free(buffer[i]);
}

/**********************************************************/
int main(void)
{
	do_allocs_and_forget_one_free();
	return EXIT_SUCCESS;
}
