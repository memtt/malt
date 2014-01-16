/********************  HEADERS  *********************/
#include <cstdio>
#include <iostream>
#include <execinfo.h>
#include <json/TypeToJson.h>
#include "AllocStackProfiler.h"
#include <common/CodeTiming.h>

/*******************  FUNCTION  *********************/
AllocStackProfiler::AllocStackProfiler(StackMode mode,bool threadSafe)
{
	this->mode = mode;
	this->threadSafe = threadSafe;

	switch(mode)
	{
		case STACK_MODE_BACKTRACE:
			stack.loadCurrentStack();
			break;
		case STACK_MODE_ENTER_EXIT_FUNC:
			exStack.enterFunction((void*)0x1);
			break;
		case STACK_MODE_USER:
			break;
	}
}

/*******************  FUNCTION  *********************/
void AllocStackProfiler::onMalloc(void* ptr, size_t size,Stack * userStack)
{
	countCalls(3,size,userStack);
}

/*******************  FUNCTION  *********************/
void AllocStackProfiler::onCalloc(void* ptr, size_t nmemb, size_t size,Stack * userStack)
{
	countCalls(3,size,userStack);
}

/*******************  FUNCTION  *********************/
void AllocStackProfiler::onFree(void* ptr,Stack * userStack)
{
	countCalls(3,0,userStack);
}

/*******************  FUNCTION  *********************/
void AllocStackProfiler::onPrepareRealloc(void* oldPtr,Stack * userStack)
{
	//nothing to unregister, skip
}

/*******************  FUNCTION  *********************/
void AllocStackProfiler::onRealloc(void* oldPtr, void* ptr, size_t newSize,Stack * userStack)
{
	countCalls(3,newSize,userStack);
}

/*******************  FUNCTION  *********************/
void AllocStackProfiler::countCalls(int skipDepth,ssize_t delta,Stack * userStack)
{
	switch(mode)
	{
		case STACK_MODE_BACKTRACE:
			CODE_TIMING("loadCurrentStack",stack.loadCurrentStack());
			OPTIONAL_CRITICAL(lock,threadSafe)
				CODE_TIMING("updateInfo",tracer.getBacktraceInfo(stack).getInfo().addEvent(delta));
			END_CRITICAL
			break;
		case STACK_MODE_ENTER_EXIT_FUNC:
			OPTIONAL_CRITICAL(lock,threadSafe)
				CODE_TIMING("updateInfoEx",tracer.getBacktraceInfo(exStack).getInfo().addEvent(delta));
			END_CRITICAL
			break;
		case STACK_MODE_USER:
			if (userStack != NULL)
			{
				OPTIONAL_CRITICAL(lock,threadSafe)
					CODE_TIMING("updateInfoEx",tracer.getBacktraceInfo(*userStack).getInfo().addEvent(delta));
				END_CRITICAL
			}
			break;
	}
}

/*******************  FUNCTION  *********************/
void AllocStackProfiler::onExit(void )
{
	puts("======================== Print on exit ========================");
	OPTIONAL_CRITICAL(lock,threadSafe)
		htopml::typeToJson(std::cout,tracer);
	END_CRITICAL
}

/*******************  FUNCTION  *********************/
void AllocStackProfiler::onEnterFunction ( void* funcAddr )
{
	assert(mode == STACK_MODE_ENTER_EXIT_FUNC);
	assert(!threadSafe);
	if (mode == STACK_MODE_ENTER_EXIT_FUNC)
		this->exStack.enterFunction(funcAddr);
}

/*******************  FUNCTION  *********************/
void AllocStackProfiler::onExitFunction ( void* funcAddr )
{
	assert(mode == STACK_MODE_ENTER_EXIT_FUNC);
	assert(!threadSafe);
	if (mode == STACK_MODE_ENTER_EXIT_FUNC)
		this->exStack.exitFunction(funcAddr);
}
