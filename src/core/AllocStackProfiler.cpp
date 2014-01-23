/********************  HEADERS  *********************/
#include <cstdio>
#include <iostream>
#include <execinfo.h>
#include <json/TypeToJson.h>
#include "AllocStackProfiler.hpp"
#include <common/CodeTiming.hpp>

/*******************  NAMESPACE  ********************/
namespace ATT
{

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
	onAllocEvent(ptr,size,3,userStack);
}

/*******************  FUNCTION  *********************/
void AllocStackProfiler::onCalloc(void* ptr, size_t nmemb, size_t size,Stack * userStack)
{
	onAllocEvent(ptr,size * nmemb,3,userStack);
}

/*******************  FUNCTION  *********************/
void AllocStackProfiler::onFree(void* ptr,Stack * userStack)
{
	if (ptr != NULL)
		onFreeEvent(ptr,3,userStack);
}

/*******************  FUNCTION  *********************/
void AllocStackProfiler::onPrepareRealloc(void* oldPtr,Stack * userStack)
{
	//nothing to unregister, skip
}

/*******************  FUNCTION  *********************/
void AllocStackProfiler::onRealloc(void* oldPtr, void* ptr, size_t newSize,Stack * userStack)
{
	ATT_OPTIONAL_CRITICAL(lock,threadSafe)
		//to avoid to search it 2 times
		SimpleCallStackNode * callStackNode = NULL;
		
		//free part
		if (ptr != NULL)
			callStackNode = onFreeEvent(oldPtr,3,userStack,callStackNode,false);
		
		//alloc part
		if (newSize > 0)
			callStackNode = onAllocEvent(ptr,newSize,3,userStack,callStackNode,false);
	ATT_END_CRITICAL
}

/*******************  FUNCTION  *********************/
SimpleCallStackNode * AllocStackProfiler::onAllocEvent(void* ptr, size_t size,int skipDepth, Stack* userStack,SimpleCallStackNode * callStackNode,bool doLock)
{
	ATT_OPTIONAL_CRITICAL(lock,threadSafe && doLock)
		//search if not provided
		if (callStackNode == NULL)
			callStackNode = getStackNode(skipDepth,size,userStack);
		
		//count events
		CODE_TIMING("updateInfoAlloc",callStackNode->getInfo().addEvent(size,0));
		
		//register for segment history tracking
		if (ptr != NULL)
			CODE_TIMING("segTracerAdd",segTracer.add(ptr,size,callStackNode));
	ATT_END_CRITICAL
	
	return callStackNode;
}

/*******************  FUNCTION  *********************/
SimpleCallStackNode * AllocStackProfiler::onFreeEvent(void* ptr,int skipDepth, Stack* userStack,SimpleCallStackNode * callStackNode,bool doLock)
{
	ATT_OPTIONAL_CRITICAL(lock,threadSafe && doLock)
		//search segment info to link with previous history
		SegmentInfo * segInfo = NULL;
		CODE_TIMING("segTracerGet",segInfo = segTracer.get(ptr));
		
		//check unknown
		if (segInfo == NULL)
		{
			//fprintf(stderr,"Caution, get unknown free segment : %p, ingore it.\n",ptr);
			return NULL;
		}
			
		ssize_t size = -segInfo->size;
		
		//search call stack info if not provided
		if (callStackNode == NULL)
			callStackNode = getStackNode(skipDepth,size,userStack);
		
		//count events
		CODE_TIMING("updateInfoFree",callStackNode->getInfo().addEvent(size,segInfo->getLifetime()));
		
		//remove tracking info
		CODE_TIMING("segTracerRemove",segTracer.remove(ptr));
	ATT_END_CRITICAL
	
	return callStackNode;
}

/*******************  FUNCTION  *********************/
SimpleCallStackNode* AllocStackProfiler::getStackNode(int skipDepth, ssize_t delta, Stack* userStack)
{
	SimpleCallStackNode * res = NULL;

	//search with selected mode
	switch(mode)
	{
		case STACK_MODE_BACKTRACE:
			CODE_TIMING("loadCurrentStack",stack.loadCurrentStack());
			CODE_TIMING("searchInfo",res = &stackTracer.getBacktraceInfo(stack));
			break;
		case STACK_MODE_ENTER_EXIT_FUNC:
			CODE_TIMING("searchInfoEx",res = &stackTracer.getBacktraceInfo(exStack));
			break;
		case STACK_MODE_USER:
			if (userStack != NULL)
				CODE_TIMING("searchInfoUser",res = &stackTracer.getBacktraceInfo(*userStack));
			break;
	}

	return res;
}

/*******************  FUNCTION  *********************/
void AllocStackProfiler::onExit(void )
{
	puts("======================== Print on exit ========================");
	ATT_OPTIONAL_CRITICAL(lock,threadSafe)
		CODE_TIMING("output",htopml::typeToJson(std::cout,stackTracer));
		ValgrindOutput vout;
		stackTracer.fillValgrindOut(vout);
		vout.writeAsCallgrind("mycallgrind.callgrind");
	ATT_END_CRITICAL
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

}
