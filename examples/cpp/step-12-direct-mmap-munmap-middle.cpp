/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 09/2025
*    LICENSE  : CeCILL-C
*    FILE     : examples/cpp/step-12-direct-mmap-munmap-middle.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
#include <cassert>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <sys/mman.h>

/**********************************************************/
void do_mmap_stuff(void)
{
	//map 1 MB
	void * ptr = mmap(NULL, 1024*1024, PROT_READ|PROT_WRITE, MAP_ANON|MAP_PRIVATE, 0, 0);
	assert(ptr != MAP_FAILED),
	memset(ptr, 1, 1024*1024);

	//unmap half in middle
	int status1 = munmap((char*)ptr+256*1024, 512*1024);
	assert(status1 == 0);

	//unmap all
	int status2 = munmap((void*)ptr, 1024*1024);
	assert(status2 == 0);
}

/**********************************************************/
int main(void)
{
	do_mmap_stuff();
	return EXIT_SUCCESS;
}
