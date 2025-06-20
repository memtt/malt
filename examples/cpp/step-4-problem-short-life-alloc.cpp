/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.3.1
*    DATE     : 02/2025
*    LICENSE  : CeCILL-C
*    FILE     : examples/cpp/step-4-problem-short-life-alloc.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <list>

/**********************************************************/
void doing_things_with_memory(void)
{
	//vars
	const size_t steps = 100000;

	//make steps
	size_t tot_len = 0;
	for (size_t step = 0 ; step < steps ; step++)
	{
		//make some computatin
		char * tmp_buffer = new char[1024];
		size_t len = snprintf(tmp_buffer, 1024, "Generating a string for step %zu", step);
		delete [] tmp_buffer;
		tot_len += len;
	}

	//final
	printf("Final string sum len = %zu\n", tot_len);
}

/**********************************************************/
int main(void)
{
	doing_things_with_memory();
	return EXIT_SUCCESS;
}
