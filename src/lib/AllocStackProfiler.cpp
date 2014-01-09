/********************  HEADERS  *********************/
#include <cstdio>
#include <iostream>
#include <execinfo.h>
#include <json/TypeToJson.h>
#include "AllocStackProfiler.h"
#include <common/CodeTiming.h>

/*******************  FUNCTION  *********************/
AllocStackProfiler::AllocStackProfiler(void)
{
	stack.loadCurrentStack();
	exStack.enterFunction((void*)0x1);
}

/*******************  FUNCTION  *********************/
void AllocStackProfiler::onMalloc(void* ptr, size_t size)
{
	countCalls(3,size);
}

/*******************  FUNCTION  *********************/
void AllocStackProfiler::onCalloc(void* ptr, size_t nmemb, size_t size)
{
	countCalls(3,size);
}

/*******************  FUNCTION  *********************/
void AllocStackProfiler::onFree(void* ptr)
{
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
	countCalls(3,newSize);
}

/*******************  FUNCTION  *********************/
void AllocStackProfiler::countCalls(int skipDepth,ssize_t delta)
{
// 	CODE_TIMING("loadCurrentStack",stack.loadCurrentStack());
// 	CODE_TIMING("updateInfo",tracer.getBacktraceInfo(stack).getInfo().addEvent(delta));
	CODE_TIMING("updateInfoEx",tracer.getBacktraceInfo(exStack).getInfo().addEvent(delta));
}

/*******************  FUNCTION  *********************/
void AllocStackProfiler::onExit(void )
{
	puts("======================== Print on exit ========================");
	htopml::typeToJson(std::cout,tracer);
}

/*******************  FUNCTION  *********************/
void AllocStackProfiler::onEnterFunction ( void* funcAddr )
{
	this->exStack.enterFunction(funcAddr);
}

/*******************  FUNCTION  *********************/
void AllocStackProfiler::onExitFunction ( void* funcAddr )
{
	this->exStack.exitFunction(funcAddr);
}
