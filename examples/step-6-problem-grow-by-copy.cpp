/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 10/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/config.h.in
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <list>

/**********************************************************/
void function_doing_things_with_memory(void)
{
	const size_t max_size = 6*1024*1024;
	char * ptr = NULL;
	size_t prev_size = 0;

	for (size_t size = 0 ; size < max_size ; size += 128)
	{
		//allocate new
		char * new_ptr = (char*)malloc(size);

		//copy (parallel)
		#pragma omp parallel for
		for (size_t i = 0 ; i < prev_size ; i++)
			new_ptr[i] = ptr[i];

		//free old
		free(ptr);

		//replace pointer
		ptr = new_ptr;
	}
}

/**********************************************************/
int main(void)
{
	function_doing_things_with_memory();
	return EXIT_SUCCESS;
}
