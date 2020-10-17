/*****************************************************
             PROJECT  : MALT
             VERSION  : 1.2.2
             DATE     : 10/2020
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <malloc.h>
#include <cstring>
#include <iostream>
#include <cassert>

/**
 * This is a simple test case to valid usage. It can be used in all modes.
 *  - STACK_MODE='enter-exit' LD_PRELOAD=../src/libAllocStackProfiler.so
 *  - LD_PRELOAD=../src/libAllocStackProfiler.so
**/

/********************* GLOBALS **********************/
//example of static and TLS arrays
int gblArray[1024];
static int gblStaticArray[1024];
const char gblString[] = "test const global string";
__thread int tlsArray[1024];

/*********************  CLASS  **********************/
class OutOfMainAlloc
{
	public:
		OutOfMainAlloc(void);
		~OutOfMainAlloc(void);
		void printf(void) {*(char*)ptr='c';};
	private:
		void * ptr;
};

/********************* GLOBALS **********************/
OutOfMainAlloc gblOutOfMainAlloc;

/*******************  FUNCTION  *********************/
OutOfMainAlloc::OutOfMainAlloc(void)
{
	ptr = malloc(128);
}

/*******************  FUNCTION  *********************/
OutOfMainAlloc::~OutOfMainAlloc(void)
{
	free(ptr);
}

/*******************  FUNCTION  *********************/
void funcC()
{
	int * ptr = new int[16];
	*(char*)ptr = 'c';//required otherwise new compilers will remove malloc/free
	delete [] ptr;
}

/*******************  FUNCTION  *********************/
void funcB()
{
	void * ptr = malloc(32);
	*(char*)ptr='c';//required otherwise new compilers will remove malloc/free
	free(ptr);
	funcC();
}

/*******************  FUNCTION  *********************/
void funcA()
{
	void * ptr = malloc(16);
	*(char*)ptr='c';//required otherwise new compilers will remove malloc/free
	free(ptr);
	funcB();
}

/*******************  FUNCTION  *********************/
void recurseA(int depth)
{
	if (depth > 0)
	{
		void * ptr = malloc(64);
		*(char*)ptr='c';//required otherwise new compilers will remove malloc/free
		recurseA(depth-1);
		free(ptr);
	}
}

/*******************  FUNCTION  *********************/
void testRealloc(void)
{
	void * ptr = realloc(NULL,16);
	ptr = realloc(ptr,32);
	ptr = realloc(ptr,0);

	ptr = malloc(44);
	*(char*)ptr = 'c';//required otherwise new compilers will remove malloc/free
	free(ptr);
}

/*******************  FUNCTION  *********************/
void testMaxAlive(void)
{
	for (int i = 0 ; i < 100 ; i++)
	{
		void * ptr1 = malloc(64);
		void * ptr2 = malloc(64);
		*(char*)ptr1='c';//required otherwise new compilers will remove malloc/free
		*(char*)ptr2='c';//required otherwise new compilers will remove malloc/free
		free(ptr1);
		free(ptr2);
	}
}

/*******************  FUNCTION  *********************/
void testThreads(void)
{
	#pragma omp parallel for
	for (int i = 0 ; i < 100 ; i++)
	{
		void * ptr = malloc(64);
		*(char*)ptr='c';//required otherwise new compilers will remove malloc/free
		free(ptr);
	}
}

/*******************  FUNCTION  *********************/
void testLeak(void)
{
	void * ptr = malloc(32);
	*(char*)ptr = 'c';
}

/*******************  FUNCTION  *********************/
void testLeak2(void)
{
	void * ptr = malloc(32);
	*(char*)ptr = 'c';
}

/*******************  FUNCTION  *********************/
void testRecuseIntervedB(int depth);
void testRecuseIntervedA(int depth)
{
	//static size_t mem = 0;
	//mem += 64;
	//printf("testRecuseIntervedA = %zu\n",mem);
	
	void * ptr = malloc(64);
	*(char*)ptr='c';//required otherwise new compilers will remove malloc/free
	free(ptr);
	testRecuseIntervedB(depth);
}

/*******************  FUNCTION  *********************/
void testRecuseIntervedB(int depth)
{
	//static size_t mem = 0;
	//mem += 64;
	//printf("testRecuseIntervedB = %zu\n",mem);
	
	void * ptr = malloc(64);
	*(char*)ptr='c';//required otherwise new compilers will remove malloc/free
	free(ptr);
	if (depth > 0)
		testRecuseIntervedA(depth-1);
}

/*******************  FUNCTION  *********************/
void testAllFuncs(void)
{
	void * ptr;
	
	//malloc
	ptr = malloc(16);
	*(char*)ptr = 'c';//required otherwise new compilers will remove malloc/free
	free(ptr);
	
	//calloc
	ptr = calloc(1,16);
	*(char*)ptr = 'c';//required otherwise new compilers will remove malloc/free
	free(ptr);
	
	//realloc
	ptr = realloc(NULL,16);
	*(char*)ptr = 'c';//required otherwise new compilers will remove malloc/free
	free(ptr);
	
	//posix_memalign
	int res = posix_memalign(&ptr,16,16);
	*(int*)ptr = res;
	free(ptr);
	
	//posix_memalign
	#ifdef aligned_alloc
		ptr = aligned_alloc(16,16);
		*(int*)ptr = res;
		free(ptr);
	#endif //aligned_alloc
	
	//posix_memalign
	ptr = memalign(16,16);
	*(int*)ptr = res;
	free(ptr);
	
	//posix_memalign
	ptr = valloc(16);
	assert(ptr != NULL);
	*(int*)ptr = res;
	free(ptr);
	
	//posix_memalign
	ptr = pvalloc(16);
	assert(ptr != NULL);
	*(int*)ptr = res;
	free(ptr);
}

/*******************  FUNCTION  *********************/
void testPeak(void)
{
	void * ptr = malloc(1024*1024);
	testRecuseIntervedA(2);
	free(ptr);
}

/*******************  FUNCTION  *********************/
void testZero(void)
{
	void * ptr = malloc(0);
	free(NULL);
	ptr = realloc(NULL,0);
	*(char*)ptr = 'c';
}

/*******************  FUNCTION  *********************/
void allocOnStack(void)
{
	char var[2048];
	memset(var,0,sizeof(var));
}

/*******************  FUNCTION  *********************/
void allocOnStackRecurse(int depth)
{
	char var[512];
	memset(var,0,sizeof(var));
	if (depth > 0)
		allocOnStackRecurse(depth-1);
}

/*******************  FUNCTION  *********************/
void testParallelWithRecurse(void)
{
	#pragma omp parallel
	testRecuseIntervedB(5);
}

/*******************  FUNCTION  *********************/
int main(void)
{
	gblArray[0] = gblString[0];
	gblStaticArray[0] = gblString[0];
	tlsArray[0] = gblArray[0];

	//memset
	memset(gblStaticArray, 0, sizeof(gblStaticArray));
	
	//ensure no remove
	printf("To not remove global variables for test : %s\n",gblString);
	
	//first is calloc
	void * ptr = calloc(16,16);
	*(char*)ptr='c';//required otherwise new compilers will remove malloc/free
	free(ptr);
	
	funcA();
	for (int i = 0 ; i < 10 ; ++i)
	{
		funcB();
	}
	recurseA(10);
	for (int i = 0 ; i < 10 ; ++i)
	{
		recurseA(10);
	}
	testRealloc();
	testMaxAlive();
	testRecuseIntervedA(2);
	testThreads();
	testAllFuncs();
	testLeak();
	testLeak2();
	testPeak();
	allocOnStack();
	allocOnStackRecurse(10);
	testParallelWithRecurse();

	return 0;
}
