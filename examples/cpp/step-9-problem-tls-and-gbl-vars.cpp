/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.6
*    DATE     : 02/2025
*    LICENSE  : CeCILL-C
*    FILE     : examples/cpp/step-9-problem-tls-and-gbl-vars.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <list>

/**********************************************************/
const size_t MBYTES = 1024UL*1024UL;
const size_t GBL_SIZE = 512 * MBYTES;
const size_t TLS_SIZE = 128 * MBYTES;

/**********************************************************/
char gblVariable[GBL_SIZE];
__thread char tlsVariable[TLS_SIZE];

/**********************************************************/
void function_doing_things_with_memory(void)
{
	char * ptr = (char*)malloc(1024*1024);
	memset(ptr, 1, 1024*1024);
	free(ptr);
	memset(gblVariable, 1, GBL_SIZE);
	#pragma omp parallel
	memset(tlsVariable, 2, TLS_SIZE);
}

/**********************************************************/
int main(void)
{
	function_doing_things_with_memory();
	return EXIT_SUCCESS;
}
