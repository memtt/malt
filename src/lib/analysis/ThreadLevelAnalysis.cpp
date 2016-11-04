/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 02/2015
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
#include "ThreadLevelAnalysis.hpp"
#include "ProcessLevelAnalysis.hpp"
#include <core/Options.hpp>
#include <json/JsonState.h>
#include <common/Debug.hpp>

#define MATT_STACK_SKIP 2

namespace MATT
{

/*********************  CONSTS  *********************/
static const char * gblMemFuncNames[] = {
	"malloc",
	"calloc",
	"realloc",
	"free",
	"memalign",
	"posix_memalign",
	"valloc",
	"pvalloc",
	"aligned_alloc",
	"ERROR_MUST_NOT_BE_USED"
};

/*******************  FUNCTION  *********************/
ThreadLevelAnalysis::ThreadLevelAnalysis ( ProcessLevelAnalysis * processLevel )
{
	this->processLevel = processLevel;
	this->stackTree = processLevel->getStackTree();
	this->inUse = false;
	this->stackTreeHandler = MATT_STACK_TREE_NULL;
}

/*******************  FUNCTION  *********************/
ThreadLevelAnalysis::~ThreadLevelAnalysis ( void )
{

}

/*******************  FUNCTION  *********************/
bool ThreadLevelAnalysis::isInUse ( void )
{
	return inUse;
}

/*******************  FUNCTION  *********************/
void ThreadLevelAnalysis::setInUse ( bool status )
{
	if (status == inUse)
		return;
	
	//action
	if (status)
	{
		this->stackTreeHandler = this->stackTree->enterThread();
	} else {
		this->stackTree->exitThread(this->stackTreeHandler);
	}
	
	//mark
	this->inUse = status;
}

/*******************  FUNCTION  *********************/
bool ThreadLevelAnalysis::mallocCallEnterExit ( void )
{
	return stackTree->isEnterExit();
}

/*******************  FUNCTION  *********************/
void ThreadLevelAnalysis::onAlignedAlloc ( MallocHooksInfos& info, void* ret, size_t alignment, size_t size )
{
	this->counters[MEM_FUNC_ALIGNED_ALLOC].call(info.calltime,size);
	this->setupStack(info,MATT_STACK_SKIP);
	this->processLevel->onAlignedAlloc(info,ret,alignment,size);
}

/*******************  FUNCTION  *********************/
void ThreadLevelAnalysis::onCalloc ( MallocHooksInfos& info, void* ret, size_t nmemb, size_t size )
{
	this->counters[MEM_FUNC_CALLOC].call(info.calltime,size);
	this->setupStack(info,MATT_STACK_SKIP);
	this->processLevel->onCalloc(info,ret,nmemb,size);
}

/*******************  FUNCTION  *********************/
void ThreadLevelAnalysis::onFree ( MallocHooksInfos& info, void* ptr )
{
	this->counters[MEM_FUNC_FREE].call(info.calltime,0);
	this->setupStack(info,MATT_STACK_SKIP);
	this->processLevel->onFree(info,ptr);
}

/*******************  FUNCTION  *********************/
void ThreadLevelAnalysis::onMalloc ( MallocHooksInfos& info, void* ret, size_t size )
{
	this->counters[MEM_FUNC_MALLOC].call(info.calltime,size);
	this->setupStack(info,MATT_STACK_SKIP);
	this->processLevel->onMalloc(info,ret,size);
}

/*******************  FUNCTION  *********************/
void ThreadLevelAnalysis::onMemalign ( MallocHooksInfos& info, void* ret, size_t alignment, size_t size )
{
	this->counters[MEM_FUNC_MEMALIGN].call(info.calltime,size);
	this->setupStack(info,MATT_STACK_SKIP);
	this->processLevel->onMemalign(info,ret,alignment,size);
}

/*******************  FUNCTION  *********************/
void ThreadLevelAnalysis::onPosixMemalign ( MallocHooksInfos& info, int ret, void** memptr, size_t align, size_t size )
{
	this->counters[MEM_FUNC_POSIX_MEMALIGN].call(info.calltime,size);
	this->setupStack(info,MATT_STACK_SKIP);
	this->processLevel->onPosixMemalign(info,ret,memptr,align,size);
}

/*******************  FUNCTION  *********************/
void ThreadLevelAnalysis::onPvalloc ( MallocHooksInfos& info, void* ret, size_t size )
{
	this->counters[MEM_FUNC_PVALLOC].call(info.calltime,size);
	this->setupStack(info,MATT_STACK_SKIP);
	this->processLevel->onPvalloc(info,ret,size);
}

/*******************  FUNCTION  *********************/
void ThreadLevelAnalysis::onValloc ( MallocHooksInfos& info, void* ret, size_t size )
{
	this->counters[MEM_FUNC_VALLOC].call(info.calltime,size);
	this->setupStack(info,MATT_STACK_SKIP);
	this->processLevel->onValloc(info,ret,size);
}

/*******************  FUNCTION  *********************/
void ThreadLevelAnalysis::onRealloc ( MallocHooksInfos& info, void* ret, void* ptr, size_t size )
{
	this->counters[MEM_FUNC_REALLOC].call(info.calltime,size);
	this->setupStack(info,MATT_STACK_SKIP);
	this->processLevel->onRealloc(info,ret,ptr,size);
}

/*******************  FUNCTION  *********************/
void ThreadLevelAnalysis::onMallocEnterFunction ( MallocHooksInfos& info  ,void * caller,void * function)
{
	stackTreeHandler = stackTree->enterFunction(stackTreeHandler,function);
	this->setupStack(info,MATT_STACK_SKIP);
	processLevel->onMallocEnterFunction(info,caller,function);
}

/*******************  FUNCTION  *********************/
void ThreadLevelAnalysis::onMallocExitFunction ( MallocHooksInfos& info ,void * caller,void * function )
{
	stackTreeHandler = stackTree->exitFunction(stackTreeHandler,function);
	this->setupStack(info,MATT_STACK_SKIP);
	processLevel->onMallocExitFunction(info,caller,function);
}

/*******************  FUNCTION  *********************/
void ThreadLevelAnalysis::onPreFree ( MallocHooksInfos& info, void* ptr )
{
	this->setupStack(info,MATT_STACK_SKIP);
	processLevel->onPreFree(info,ptr);
}

/*******************  FUNCTION  *********************/
void ThreadLevelAnalysis::onPreRealloc ( MallocHooksInfos& info, void* ptr, size_t size )
{
	this->setupStack(info,MATT_STACK_SKIP);
	processLevel->onPreRealloc(info,ptr,size);
}

/*******************  FUNCTION  *********************/
void convertToJson ( htopml::JsonState& json, const ThreadLevelAnalysis& value )
{
	json.openStruct();
		json.openFieldStruct("stats");
			for (int i = 0 ; i < MEM_FUNC_COUNT ; i++)
				json.printField(gblMemFuncNames[i],value.counters[i]);
		json.closeFieldStruct("stats");
	json.closeStruct();
}

/*******************  FUNCTION  *********************/
bool ThreadLevelAnalysis::isEnterExitFunction ( void )
{
	return stackTree->isEnterExit();
}

/*******************  FUNCTION  *********************/
void ThreadLevelAnalysis::onEnterFunction ( void* caller, void* function )
{
	stackTreeHandler = stackTree->enterFunction(stackTreeHandler,function);
	//processLevel->onEnterFunction(caller,function);
}

/*******************  FUNCTION  *********************/
void ThreadLevelAnalysis::onExitFunction ( void* caller, void* function )
{
	stackTreeHandler = stackTree->exitFunction(stackTreeHandler,function);
	//processLevel->onExitFunction(caller,function);
}

/*******************  FUNCTION  *********************/
void ThreadLevelAnalysis::setupStack ( MallocHooksInfos& info, int skip )
{
	if (isEnterExitFunction() == false)
		stackTreeHandler = stackTree->getFromStack(stackTreeHandler,skip+1);
	info.handler = stackTreeHandler;
	info.dataHandler = stackTree->getDataHandler(info.handler);
	info.dataId = stackTree->getStackId(info.dataHandler);
}

}
