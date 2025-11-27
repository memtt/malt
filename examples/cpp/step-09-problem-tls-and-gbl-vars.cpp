/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 02/2025
*    LICENSE  : CeCILL-C
*    FILE     : examples/cpp/step-09-problem-tls-and-gbl-vars.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <list>
#include <thread>
#include <omp.h>

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
	const size_t size = 1024*1024;
	char * ptr = (char*)malloc(size);
	memset(ptr, 1, size);
	free(ptr);
	memset(gblVariable, 1, GBL_SIZE);
	#pragma omp parallel
	memset(tlsVariable, 2, TLS_SIZE);
}

/**********************************************************/
//to check if not accounted in max thread number alive
void function_spawning_threads_one_at_a_time()
{
	size_t omp_threads = omp_get_max_threads();
	for (size_t i = 0 ; i < omp_threads * 2 ; i++) {
		printf("Spawning thread %zu\n", i);
		std::thread thread([]{
			void * ptr = malloc(1);
			free(ptr);
		});
		thread.join();
	}
}

/**********************************************************/
int main(void)
{
	function_doing_things_with_memory();
	function_spawning_threads_one_at_a_time();
	return EXIT_SUCCESS;
}
