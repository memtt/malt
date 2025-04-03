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
void function_peak_1()
{
	//peak 1 at 1MB
	void * ptrs[1024];
	for (size_t i = 0 ; i < 1024 ; i ++)
		ptrs[i] = malloc(1024);
	for (size_t i = 0 ; i < 1024 ; i ++)
		free(ptrs[i]);
}

/**********************************************************/
void function_peak_2()
{
	//peak 1 at 10MB
	void * ptrs[1024];
	for (size_t i = 0 ; i < 1024 ; i ++)
		ptrs[i] = malloc(10240);
	for (size_t i = 0 ; i < 1024 ; i ++)
		free(ptrs[i]);
}

/**********************************************************/
void function_doing_things_with_memory(void)
{
	function_peak_1();
	function_peak_2();
}

/**********************************************************/
int main(void)
{
	function_doing_things_with_memory();
	return EXIT_SUCCESS;
}
