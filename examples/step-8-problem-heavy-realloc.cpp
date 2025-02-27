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
	char * ptr = nullptr;
	const size_t delta = 256;
	for (size_t i = 0 ; i < 1024*1024 ; i += delta) {
		ptr = (char*)realloc(ptr, i);
		for (size_t j = i - delta ; j < i ; j++)
			ptr[j] = i/delta;
	}
}

/**********************************************************/
int main(void)
{
	function_doing_things_with_memory();
	return EXIT_SUCCESS;
}
