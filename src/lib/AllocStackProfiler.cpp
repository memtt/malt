/********************  HEADERS  *********************/
#include <execinfo.h>
#include <iostream>
#include <cstdio>
#include <json/TypeToJson.h>
#include "AllocStackProfiler.h"

/*******************  FUNCTION  *********************/
void AllocStackProfiler::init(void )
{
	this->tracer = new SimpleTracer();
}

/*******************  FUNCTION  *********************/
void AllocStackProfiler::onMalloc(void* ptr, size_t size)
{
	//trace finished
	if (tracer == NULL)
		return;

	countCalls(3);
}

/*******************  FUNCTION  *********************/
void AllocStackProfiler::onCalloc(void* ptr, size_t nmemb, size_t size)
{
	//trace finished
	if (tracer == NULL)
		return;

	countCalls(3);
}

/*******************  FUNCTION  *********************/
void AllocStackProfiler::onFree(void* ptr)
{
	//trace finished
	if (tracer == NULL)
		return;

	countCalls(3);
}

/*******************  FUNCTION  *********************/
void AllocStackProfiler::onPrepareRealloc(void* oldPtr)
{
	//nothing to unregister, skip
}

/*******************  FUNCTION  *********************/
void AllocStackProfiler::onRealloc(void* oldPtr, void* ptr, size_t newSize)
{
	//trace finished
	if (tracer == NULL)
		return;

	countCalls(3);
}

/*******************  FUNCTION  *********************/
void AllocStackProfiler::countCalls(int skipDepth)
{
	void * buffer[1024];
	int size = backtrace(buffer,sizeof(buffer)/sizeof(void*));
	assert(size > 0);
	tracer->getBacktraceInfo(buffer+skipDepth,size-skipDepth).incrCnt();
}

/*******************  FUNCTION  *********************/
void AllocStackProfiler::onExit(void )
{
	SimpleTracer * oldTracer = tracer;
	tracer = NULL;
	puts("Print on exit ========================");
	htopml::typeToJson(std::cout,*oldTracer);
}
