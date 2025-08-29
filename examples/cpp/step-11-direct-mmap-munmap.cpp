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

	//unmap half
	int status1 = munmap(ptr, 512*1024);
	assert(status1 == 0);

	//unmap half
	int status2 = munmap((void*)((char*)ptr + 512*1024), 512*1024);
	assert(status2 == 0);
}

/**********************************************************/
void do_mremap_stuff(void)
{
	//map 1 MB
	void * ptr = mmap(NULL, 1024*1024, PROT_READ|PROT_WRITE, MAP_ANON|MAP_PRIVATE, 0, 0);
	assert(ptr != MAP_FAILED),
	memset(ptr, 1, 1024*1024);

	//unmap half
	ptr = mremap(ptr, 1024*1024, 512*1024, 0);
	assert(ptr != MAP_FAILED);

	//unmap half
	int status1 = munmap(ptr, 512*1024);
	assert(status1 == 0);
}

/**********************************************************/
int main(void)
{
	do_mmap_stuff();
	do_mremap_stuff();
	return EXIT_SUCCESS;
}
