/********************  HEADERS  *********************/
#include <execinfo.h>
#include <iostream>
#include <cassert>
#include <cstdlib>
#include <dlfcn.h>
#include "json/TypeToJson.h"
#include "SimpleTracer.h"

/********************  GLOBALS  **********************/
SimpleTracer tracer;

/*******************  FUNCTION  *********************/
typedef void * (*MallocFuncPtr)(size_t size);

/*******************  FUNCTION  *********************/
void countCalls(void)
{
	void * buffer[1024];
	int size = backtrace(buffer,sizeof(buffer)/sizeof(void*));
	assert(size > 0);
	tracer.getBacktraceInfo(buffer+1,size-1).incrCnt();
}

/*******************  FUNCTION  *********************/
//trace malloc
void * malloc(size_t size)
{
	static MallocFuncPtr oldMalloc = NULL;
	static bool inMalloc = false;

	//check old symbol
	if (oldMalloc == NULL)
		oldMalloc = (MallocFuncPtr)dlsym(RTLD_NEXT,"malloc");

	//count
	if (!inMalloc)
	{
		inMalloc = true;
		countCalls();
		inMalloc = false;
	}

	//do alloc
	return oldMalloc(size);
}

/*******************  FUNCTION  *********************/
void funcC(void)
{
	countCalls();
}

/*******************  FUNCTION  *********************/
void funcB(void)
{
	countCalls();
	funcC();
}

/*******************  FUNCTION  *********************/
void funcA(void)
{
	countCalls();
	funcB();
}

/*******************  FUNCTION  *********************/
int main(int argc, char **argv)
{
	for (int i = 0 ; i < 4 ; i++)
		funcA();
	funcB();

	htopml::typeToJson(std::cout,tracer);
	return 0;
}
