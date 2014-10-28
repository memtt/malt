/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
//locals from common/
#include <common/Debug.hpp>
#include <common/CodeTiming.hpp>
//Unix for mmap flags (need to move to portability directory)
#include <sys/mman.h>
//locals
#include "LocalAllocStackProfiler.hpp"

/*******************  NAMESPACE  ********************/
namespace MATT
{

/*******************  FUNCTION  *********************/
LocalAllocStackProfiler::LocalAllocStackProfiler(AllocStackProfiler* globalProfiler, bool reentrance)
{
	//errors
	assert(globalProfiler != NULL);
	
	//mark in use to avoid reentrance in cas of usage of malloc in future to setup fields
	this->reentrance = reentrance;
	this->inUse = true;
	
	//setup fields
	this->cntMemOps = 0;
	this->cumulAlloc = 0;
	this->globalProfiler = globalProfiler;
	this->options = globalProfiler->getOptions();
	this->stackMode = globalProfiler->getStackMode();
	this->enterExitStackTracer = globalProfiler->getEnterExitStackTracer();
	enterExitStack.enterFunction((void*)0x1);
	backtraceStack.loadCurrentStack();
	
	this->enterExitHandler = enterExitStackTracer->getRoot();
	
	//ok for use
	this->inUse = false;
}

/*******************  FUNCTION  *********************/
LocalAllocStackProfiler::~LocalAllocStackProfiler(void)
{
	//nothing up to now
}

/*******************  FUNCTION  *********************/
void LocalAllocStackProfiler::onMalloc(void* res, size_t size)
{
	//old state
	bool oldInuse = inUse;
	
	//check for reentrance
	if (!reentrance || !oldInuse)
	{
		inUse = true;
		CODE_TIMING("mallocProf",globalProfiler->onMalloc(res,size,getStack()));
		this->cntMemOps++;
		this->cumulAlloc+=size;
		inUse = oldInuse;
	}
}

/*******************  FUNCTION  *********************/
void LocalAllocStackProfiler::onFree(void* ptr)
{
	//old state
	bool oldInuse = inUse;
	
	//check for reentrance
	if (!reentrance || !oldInuse)
	{
		inUse = true;
		CODE_TIMING("freeProf",globalProfiler->onFree(ptr,getStack()));
		this->cntMemOps++;
		inUse = oldInuse;
	}
}

/*******************  FUNCTION  *********************/
void LocalAllocStackProfiler::onCalloc(void * res,size_t nmemb, size_t size)
{
	//old state
	bool oldInuse = inUse;
	
	//check for reentrance
	if (!reentrance || !oldInuse)
	{
		inUse = true;
		CODE_TIMING("callocProf",globalProfiler->onCalloc(res,nmemb,size,getStack()));
		this->cntMemOps++;
		this->cumulAlloc+=size;
		inUse = oldInuse;
	}
}

/*******************  FUNCTION  *********************/
void LocalAllocStackProfiler::onRealloc(void* ptr, void* res, size_t size)
{
	//old state
	bool oldInuse = inUse;
	
	//check for reentrance
	if (!reentrance || !oldInuse)
	{
		inUse = true;
		CODE_TIMING("reallocProf",globalProfiler->onRealloc(ptr,res,size,getStack()));
		this->cntMemOps++;
		this->cumulAlloc+=size;
		inUse = oldInuse;
	}
}

/*******************  FUNCTION  *********************/
void LocalAllocStackProfiler::onMmap(void* ptr, size_t size, int flags, int fd)
{
// 	//skip file and non anon
// 	if ((flags & (MAP_ANON|MAP_ANONYMOUS)) == 0 || fd != 0)
// 		return;
// 	
// 	//old state
// 	bool oldInuse = inUse;
// 	
// 	//get stack
// 	Stack * stack = getStack();
// 	
// 	//check for renentrance
// 	if (!reentrance || !oldInuse)
// 	{
// 		inUse = true;
// 		CODE_TIMING("userMmapProf",globalProfiler->onMalloc(ptr,size,stack));
// 		this->cntMemOps++;
// 		this->cumulAlloc+=size;
// 		inUse = oldInuse;
// 	}
	
	//CODE_TIMING("globalMmapProf",globalProfiler->onGlobalMmap(ptr,size,stack));
}

/*******************  FUNCTION  *********************/
void LocalAllocStackProfiler::onMunmap(void* ptr, size_t size)
{
// 	//old state
// 	bool oldInuse = inUse;
// 	
// 	//get stack
// 	Stack * stack = getStack();
// 	
// 	//check for renentrance
// 	if (!reentrance || !oldInuse)
// 	{
// 		inUse = true;
// 		CODE_TIMING("userMmapProf",globalProfiler->onFree(ptr,stack));
// 		this->cntMemOps++;
// 		this->cumulAlloc+=size;
// 		inUse = oldInuse;
// 	}
	
	//CODE_TIMING("globalMmapProf",globalProfiler->onGlobalMunmap(ptr,size,stack));
}

/*******************  FUNCTION  *********************/
void convertToJson(htopml::JsonState& json, const LocalAllocStackProfiler& value)
{
	json.openStruct();
	json.printField("stackMem",value.stackSizeAnalyser);
	json.printField("cntMemOps",value.cntMemOps);
	json.printField("cumulAlloc",value.cumulAlloc);
	json.closeStruct();
}

/*******************  FUNCTION  *********************/
void LocalAllocStackProfiler::resolveSymbols(SymbolResolver& symbolResolver) const
{
	this->stackSizeAnalyser.resolveSymbols(symbolResolver);
}

/*******************  FUNCTION  *********************/
Stack* LocalAllocStackProfiler::getStack(void )
{
	//search with selected mode
	switch(stackMode)
	{
		case STACK_MODE_BACKTRACE:
			CODE_TIMING("loadCurrentStack",backtraceStack.loadCurrentStack());
			backtraceStack.fastSkip(3);
			return &backtraceStack;
		case STACK_MODE_ENTER_EXIT_FUNC:
			return &enterExitStack;
		case STACK_MODE_USER:
			return NULL;
		default:
			MATT_FATAL("Invalid stack mode !");
			return NULL;
	}
}

/*******************  FUNCTION  *********************/
bool LocalAllocStackProfiler::isEnterExit(void)
{
	return stackMode == STACK_MODE_ENTER_EXIT_FUNC;
}

}
