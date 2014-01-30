/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
//standard
#include <cstdio>
#include <fstream>
#include <iostream>
//extension GNU
#include <execinfo.h>
//from htopml
#include <json/ConvertToJson.h>
#include <portability/OS.hpp>
//internals
#include <common/CodeTiming.hpp>
#include <common/FormattedMessage.hpp>
#include "AllocStackProfiler.hpp"

/*******************  NAMESPACE  ********************/
namespace MATT
{

/*******************  FUNCTION  *********************/
AllocStackProfiler::AllocStackProfiler(const Options & options,StackMode mode,bool threadSafe)
	:requestedMem(options.timeProfilePoints,options.timeProfileLinear)
{
	this->mode = mode;
	this->threadSafe = threadSafe;
	this->options = options;

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
	MATT_OPTIONAL_CRITICAL(lock,threadSafe)
		//to avoid to search it 2 times
		SimpleCallStackNode * callStackNode = NULL;
		
		//free part
		if (ptr != NULL)
			callStackNode = onFreeEvent(oldPtr,3,userStack,callStackNode,false);
		
		//alloc part
		if (newSize > 0)
			callStackNode = onAllocEvent(ptr,newSize,3,userStack,callStackNode,false);
	MATT_END_CRITICAL
}

/*******************  FUNCTION  *********************/
SimpleCallStackNode * AllocStackProfiler::onAllocEvent(void* ptr, size_t size,int skipDepth, Stack* userStack,SimpleCallStackNode * callStackNode,bool doLock)
{
	MATT_OPTIONAL_CRITICAL(lock,threadSafe && doLock)
		//update mem usage
		if (options.doTimeProfile)
		{
			requestedMem.onDeltaEvent(size);
			if (virtualMem.isNextPoint())
			{
				OSMemUsage mem = OS::getMemoryUsage();
				virtualMem.onUpdateValue(mem.virtualMemory);
				physicalMem.onUpdateValue(mem.physicalMemory);
			}
		}
	
		if (options.doStackProfile)
		{
			//search if not provided
			if (callStackNode == NULL)
				callStackNode = getStackNode(skipDepth,size,userStack);
			
			//count events
			CODE_TIMING("updateInfoAlloc",callStackNode->getInfo().onAllocEvent(size));
		}

		//register for segment history tracking
		if (ptr != NULL)
			CODE_TIMING("segTracerAdd",segTracer.add(ptr,size,callStackNode));
	MATT_END_CRITICAL
	
	return callStackNode;
}

/*******************  FUNCTION  *********************/
SimpleCallStackNode * AllocStackProfiler::onFreeEvent(void* ptr,int skipDepth, Stack* userStack,SimpleCallStackNode * callStackNode,bool doLock)
{
	MATT_OPTIONAL_CRITICAL(lock,threadSafe && doLock)
		//update memory usage
		if (options.doTimeProfile && virtualMem.isNextPoint())
		{
			OSMemUsage mem = OS::getMemoryUsage();
			virtualMem.onUpdateValue(mem.virtualMemory);
			physicalMem.onUpdateValue(mem.physicalMemory);
		}

		//search segment info to link with previous history
		SegmentInfo * segInfo = NULL;
		if (options.doTimeProfile || options.doStackProfile)
			CODE_TIMING("segTracerGet",segInfo = segTracer.get(ptr));
		
		//check unknown
		if (segInfo == NULL)
		{
			//fprintf(stderr,"Caution, get unknown free segment : %p, ingore it.\n",ptr);
			return NULL;
		}
			
		//update mem usage
		ssize_t size = -segInfo->size;
		if (options.doTimeProfile)
			requestedMem.onDeltaEvent(size);
		
		if (options.doStackProfile)
		{
			//search call stack info if not provided
			if (callStackNode == NULL)
				callStackNode = getStackNode(skipDepth,size,userStack);
			
			//count events
			CODE_TIMING("updateInfoFree",callStackNode->getInfo().onFreeEvent(size,segInfo->getLifetime()));
		}
		
		//remove tracking info
		CODE_TIMING("segTracerRemove",segTracer.remove(ptr));
	MATT_END_CRITICAL
	
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
	MATT_OPTIONAL_CRITICAL(lock,threadSafe)
		//open output file
		//TODO manage errors
		std::ofstream out;
		
		//lua
		out.open(FormattedMessage(options.outputFile).arg(OS::getExeName()).arg(OS::getPID()).arg("lua").toString().c_str());
		CODE_TIMING("outputLua",htopml::convertToLua(out,*this,options.indent));
		out.close();

		//json
		out.open(FormattedMessage(options.outputFile).arg(OS::getExeName()).arg(OS::getPID()).arg("json").toString().c_str());
		CODE_TIMING("outputJson",htopml::convertToJson(out,*this,options.indent));
		out.close();

		//valgrind out
		ValgrindOutput vout;
		stackTracer.fillValgrindOut(vout);
		CODE_TIMING("outputCallgrind",vout.writeAsCallgrind(FormattedMessage(options.outputFile).arg(OS::getExeName()).arg(OS::getPID()).arg("callgrind").toString()));
	MATT_END_CRITICAL
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

/*******************  FUNCTION  *********************/
void convertToJson(htopml::JsonState& json, const AllocStackProfiler& value)
{
	json.openStruct();
	if (value.options.doStackProfile)
		json.printField("stackInfo",value.stackTracer);
	if (value.options.doTimeProfile)
	{
		json.printField("requestedMem",value.requestedMem);
		json.printField("physicalMem",value.physicalMem);
		json.printField("virtualMem",value.virtualMem);
	}
	json.printField("ticksPerSecond",ticksPerSecond());
	json.closeStruct();
}

}
