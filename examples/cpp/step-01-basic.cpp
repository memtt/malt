/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 08/2025
*    LICENSE  : CeCILL-C
*    FILE     : examples/cpp/step-01-basic.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
/*
This file provide a basic example for MALT by making different kind
of memory allocations, about :
 - Global variables
 - Thread Local Storage (TLS)
 - malloc / free
 - new / delete
*/

/**********************************************************/
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <list>
#include <thread>
#include <unistd.h>

/**********************************************************/
// Here a global varaible
static char buffer[1024*1024*64];

/**********************************************************/
// Here a TLS (Thread Local Storage), it means building an instance
// of the global variable per thread plus the original copy with the
// init data to copy from when spwawning the thread.
static char __thread * thread_private_buffer[8*1024*1024];

/**********************************************************/
/**
 * Here a simple function using malloc / free and using the memory by
 * setting it to 0.
 */
void function_using_c_malloc(void)
{
	//alloc
	const size_t size = 1024 * 1024;
	void * ptr = malloc(size);

	//init
	memset(ptr, 0, size);

	//free
	free(ptr);
}

/**********************************************************/
void function_using_cpp_new(void)
{
	//alloc
	const size_t size = 1024 * 1024;
	double * data = new double[size];

	//init
	for (size_t i = 0 ; i < size ; ++i)
		data[i] = 11.5;

	//free
	delete [] data;
}

/**********************************************************/
void function_using_indirect_allocs_printf(void)
{
	printf("When I'm printing the lib is making some mallocs : %d\n",1);
}

/**********************************************************/
void function_using_indirect_allocs_std_list(void)
{
	//create list
	const size_t nb = 10*1000;
	std::list<int> elements;

	//fill
	for (size_t i = 0 ; i < nb ; i++)
		elements.push_back(i);
}

/**********************************************************/
class ClassWithOnlyFuncs
{
	public:
		ClassWithOnlyFuncs(void) {};
		~ClassWithOnlyFuncs(void) {};
};

/**********************************************************/
template <class T>
void build_fill_free(void)
{
	//pointer array
	const size_t nb = 1000;
	T * elements[nb];

	//allocate all
	for (size_t i = 0 ; i < nb ; i++)
		elements[i] = new T;

	//free
	for (size_t i = 0 ; i < nb ; i++)
		delete elements[i];
}

/**********************************************************/
void function_template_zero_allocs(void)
{
	build_fill_free<ClassWithOnlyFuncs>();
}

/**********************************************************/
void function_spawn_threads(void)
{
	memset(buffer, 0, sizeof(buffer));
	#pragma omp parallel
	memset(thread_private_buffer, 0, sizeof(thread_private_buffer));
}

/**********************************************************/
void function_range_many_size(void)
{
	const size_t mem = 1024;
	//loop over block size
	for (size_t block_size = 1 ; block_size < mem ; block_size *= 2)
	{
		//store pointers
		void ** ptr = new void*[mem];

		//allocate all blocks to feed 1 MB
		for (size_t i = 0 ; i < mem / block_size ; i++)
		{
			ptr[i] = calloc(block_size, 1);
			memset(ptr[i], 0, block_size);
		}

		//free all of them
		for (size_t i = 0 ; i < mem / block_size ; i++)
			free(ptr[i]);
	}
}

/**********************************************************/
void function_realloc(void)
{
	const size_t max_size = 64;
	void * ptr = NULL;
	for (size_t i = 1 ; i < max_size ; i++)
		ptr = realloc(ptr, i);
	free(ptr);
}

/**********************************************************/
void function_stack_mem_usage_recurse(int cnt, char * parent_buffer)
{
	if (cnt > 0) {
		char buffer[256];
		memcpy(buffer, parent_buffer, sizeof(buffer));
		usleep(100);
		function_stack_mem_usage_recurse(cnt - 1, buffer);
		usleep(100);
	}
}

/**********************************************************/
void function_stack_mem_usage(void)
{
	char buffer[32*1024];
	memset(buffer, 0, sizeof(buffer));

	//run locally
	usleep(200);

	#pragma omp parallel for
	for (size_t i = 0 ; i < 4 ; i++)
		function_stack_mem_usage_recurse(10, buffer);
}

/**********************************************************/
int main(void)
{
	//call all
	function_stack_mem_usage();
	function_using_c_malloc();
	function_using_cpp_new();
	function_using_indirect_allocs_printf();
	function_using_indirect_allocs_std_list();
	function_template_zero_allocs();
	function_range_many_size();
	function_realloc();

	//ok
	return EXIT_SUCCESS;
}
