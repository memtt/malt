/********************  HEADERS  *********************/
#include <cstdio>
#include <iostream>
#include <execinfo.h>
#include <json/TypeToJson.h>
#include "AllocStackProfiler.h"

/*******************  FUNCTION  *********************/
AllocStackProfiler::AllocStackProfiler(void)
{
}

/*******************  FUNCTION  *********************/
void AllocStackProfiler::init(void )
{
	this->tracer = new SimpleStackTracer();
}

/*******************  FUNCTION  *********************/
void AllocStackProfiler::onMalloc(void* ptr, size_t size)
{
	//trace finished
	if (tracer == NULL)
		return;

	countCalls(3,size);
}

/*******************  FUNCTION  *********************/
void AllocStackProfiler::onCalloc(void* ptr, size_t nmemb, size_t size)
{
	//trace finished
	if (tracer == NULL)
		return;

	countCalls(3,size);
}

/*******************  FUNCTION  *********************/
void AllocStackProfiler::onFree(void* ptr)
{
	//trace finished
	if (tracer == NULL)
		return;

	countCalls(3,0);
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

	countCalls(3,newSize);
}

/*******************  FUNCTION  *********************/
void AllocStackProfiler::countCalls(int skipDepth,ssize_t delta)
{
	stack.loadCurrentStack();
	tracer->getBacktraceInfo(stack).getInfo().addEvent(delta);
}

/*******************  FUNCTION  *********************/
void AllocStackProfiler::onExit(void )
{
	if (tracer != NULL)
	{
		SimpleStackTracer * oldTracer = tracer;
		tracer = NULL;
		puts("Print on exit ========================");
		htopml::typeToJson(std::cout,*oldTracer);
		delete oldTracer;
	} else {
		puts("Get NULL tracer");
	}
}
