/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 10/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/profiler/LocalAllocStackProfiler.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2014 - 2024
*    AUTHOR   : Sébastien Valat (ECR) - 2014
***********************************************************/

/**********************************************************/
//locals from common/
#include <common/Debug.hpp>
#include <common/CodeTiming.hpp>
//Unix for mmap flags (need to move to portability directory)
#include <sys/mman.h>
//locals
#include "LocalAllocStackProfiler.hpp"

/**********************************************************/
namespace MALT
{

/**********************************************************/
LocalAllocStackProfiler::LocalAllocStackProfiler(AllocStackProfiler* globalProfiler, bool reentrance)
	:backtracePythonStack(globalProfiler->getPythonSymbolTracker())
{
	//errors
	assert(globalProfiler != NULL);
	
	//mark in use to avoid reentrance in cas of usage of malloc in future to setup fields
	this->reentrance = reentrance;
	this->inUse = true;
	
	//setup fields
	this->cntMemOps = 0;
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

/**********************************************************/
LocalAllocStackProfiler::~LocalAllocStackProfiler(void)
{
	//nothing up to now
}

/**********************************************************/
//TODO: Pass Stack*, may be nullptr for C CallStack
//TODO: Pass Stack* to getStack()
void LocalAllocStackProfiler::onMalloc(void* res, size_t size, ticks time, MallocKind kind, Language lang)
{
	//old state
	bool oldInuse = inUse;
	
	//check for reentrance
	if (!reentrance || !oldInuse)
	{
		inUse = true;
		CODE_TIMING("mallocProf",globalProfiler->onMalloc(res,size,getStack(lang)));
		this->cntMemOps++;
		this->allocStats.malloc[kind].inc(size,time);
		inUse = oldInuse;
	}
}

/**********************************************************/
void LocalAllocStackProfiler::onFree(void* ptr, ticks time, Language lang)
{
	//old state
	bool oldInuse = inUse;
	
	//check for reentrance
	if (!reentrance || !oldInuse)
	{
		inUse = true;
		CODE_TIMING("freeProf",globalProfiler->onFree(ptr,getStack(lang)));
		this->cntMemOps++;
		inUse = oldInuse;
		this->allocStats.free.inc(0,time);
	}
}

/**********************************************************/
void LocalAllocStackProfiler::onCalloc(void * res,size_t nmemb, size_t size, ticks time, Language lang)
{
	//old state
	bool oldInuse = inUse;
	
	//check for reentrance
	if (!reentrance || !oldInuse)
	{
		inUse = true;
		CODE_TIMING("callocProf",globalProfiler->onCalloc(res,nmemb,size,getStack(lang)));
		this->cntMemOps++;
		this->allocStats.calloc.inc(size,time);
		inUse = oldInuse;
	}
}

/**********************************************************/
void LocalAllocStackProfiler::onRealloc(void* ptr, void* res, size_t size,ticks time, Language lang)
{
	//old state
	bool oldInuse = inUse;
	
	//check for reentrance
	if (!reentrance || !oldInuse)
	{
		inUse = true;
		CODE_TIMING("reallocProf",globalProfiler->onRealloc(ptr,res,size,getStack(lang)));
		this->cntMemOps++;
		this->allocStats.realloc.inc(size,time);
		inUse = oldInuse;
	}
}

/**********************************************************/
void LocalAllocStackProfiler::onMmap(void* ptr, size_t size, int flags, int fd)
{
	//skip file and non anon
	if ((flags & (MAP_ANON|MAP_ANONYMOUS)) == 0 || fd != 0)
		return;
	
	//old state
	bool oldInuse = inUse;
	
	//get stack
	Stack * stack = getStack();
	
	//check for renentrance
	if (!reentrance || !oldInuse)
	{
		inUse = true;
		CODE_TIMING("userMmapProf",globalProfiler->onMmap(ptr,size,stack));
		inUse = oldInuse;
	}
	
	//CODE_TIMING("globalMmapProf",globalProfiler->onGlobalMmap(ptr,size,stack));
}

/**********************************************************/
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

/**********************************************************/
void convertToJson(htopml::JsonState& json, const LocalAllocStackProfiler& value)
{
	json.openStruct();
	json.printField("stackMem",value.stackSizeAnalyser);
	json.printField("cntMemOps",value.cntMemOps);
	json.printField("stats",value.allocStats);
	json.closeStruct();
}

/**********************************************************/
void LocalAllocStackProfiler::solveSymbols(SymbolSolver& symbolResolver) const
{
	this->stackSizeAnalyser.solveSymbols(symbolResolver);
}

/**********************************************************/
//TODO: getStack should receive a parameter iff it's a Python stack
Stack* LocalAllocStackProfiler::getStack(Language lang)
{
	if (lang == LANG_C) {
		//search with selected mode
		switch(stackMode)
		{
			case STACK_MODE_BACKTRACE:
				CODE_TIMING("loadCurrentStack",backtraceStack.loadCurrentStack());
				backtraceStack.fastSkip(gblOptions->stackSkip);
				return &backtraceStack;
			case STACK_MODE_ENTER_EXIT_FUNC:
				return &enterExitStack;
			case STACK_MODE_USER:
				return NULL;
			default:
				MALT_FATAL("Invalid stack mode !");
				return NULL;
		}
	} else if (lang == LANG_PYTHON) {
		//search with selected mode
		switch(stackMode)
		{
			case STACK_MODE_BACKTRACE:
				CODE_TIMING("loadCurrentStack",backtracePythonStack.loadCurrentStack());
				return &backtracePythonStack;
			case STACK_MODE_ENTER_EXIT_FUNC:
				return &enterExitStack;
			case STACK_MODE_USER:
				return NULL;
			default:
				MALT_FATAL("Invalid stack mode !");
				return NULL;
		}
	} else {
		MALT_FATAL("Invalid language");
		return NULL;
	}
}

/**********************************************************/
bool LocalAllocStackProfiler::isEnterExit(void)
{
	return stackMode == STACK_MODE_ENTER_EXIT_FUNC;
}

/**********************************************************/
FunctionStat::FunctionStat(void)
{
	this->count = 0;
	this->sum = 0;
	this->time = 0;
}

/**********************************************************/
void FunctionStat::inc(ssize_t value,ticks time)
{
	this->count++;
	this->sum += value;
	this->time += time;
}

/**********************************************************/
void convertToJson(htopml::JsonState& json, const FunctionStat& value)
{
	json.openStruct();
	json.printField("count",value.count);
	json.printField("sum",value.sum);
	json.printField("time",value.time);
	json.closeStruct();
}

/**********************************************************/
void convertToJson(htopml::JsonState& json, const PerThreadAllocStats& value)
{
	json.openStruct();
	json.printField("malloc",value.malloc[MALLOC_KIND_MALLOC]);
	json.printField("posix_memalign",value.malloc[MALLOC_KIND_POSIX_MEMALIGN]);
	json.printField("aligned_alloc",value.malloc[MALLOC_KIND_ALIGNED_ALLOC]);
	json.printField("memalign",value.malloc[MALLOC_KIND_MEMALIGN]);
	json.printField("valloc",value.malloc[MALLOC_KIND_VALLOC]);
	json.printField("pvalloc",value.malloc[MALLOC_KIND_PVALLOC]);
	json.printField("free",value.free);
	json.printField("calloc",value.calloc);
	json.printField("realloc",value.realloc);
	json.closeStruct();
}

/**********************************************************/
bool LocalAllocStackProfiler::markInUseAndGetOldStatus(void)
{
	bool old = this->inUse;
	this->inUse = true;
	return old;
}

/**********************************************************/
void LocalAllocStackProfiler::restoreInUseStatus(bool oldStatus)
{
	this->inUse = oldStatus;
}

}
