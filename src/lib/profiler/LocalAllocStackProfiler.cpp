/*****************************************************
             PROJECT  : MALT
             VERSION  : 1.2.2
             DATE     : 10/2020
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
namespace MALT
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
void LocalAllocStackProfiler::onMalloc(void* res, size_t size, ticks time, MallocKind kind)
{
	//old state
	bool oldInuse = inUse;
	
	//check for reentrance
	if (!reentrance || !oldInuse)
	{
		inUse = true;
		CODE_TIMING("mallocProf",globalProfiler->onMalloc(res,size,getStack()));
		this->cntMemOps++;
		this->allocStats.malloc[kind].inc(size,time);
		inUse = oldInuse;
	}
}

/*******************  FUNCTION  *********************/
void LocalAllocStackProfiler::onFree(void* ptr, ticks time)
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
		this->allocStats.free.inc(0,time);
	}
}

/*******************  FUNCTION  *********************/
void LocalAllocStackProfiler::onCalloc(void * res,size_t nmemb, size_t size, ticks time)
{
	//old state
	bool oldInuse = inUse;
	
	//check for reentrance
	if (!reentrance || !oldInuse)
	{
		inUse = true;
		CODE_TIMING("callocProf",globalProfiler->onCalloc(res,nmemb,size,getStack()));
		this->cntMemOps++;
		this->allocStats.calloc.inc(size,time);
		inUse = oldInuse;
	}
}

/*******************  FUNCTION  *********************/
void LocalAllocStackProfiler::onRealloc(void* ptr, void* res, size_t size,ticks time)
{
	//old state
	bool oldInuse = inUse;
	
	//check for reentrance
	if (!reentrance || !oldInuse)
	{
		inUse = true;
		CODE_TIMING("reallocProf",globalProfiler->onRealloc(ptr,res,size,getStack()));
		this->cntMemOps++;
		this->allocStats.realloc.inc(size,time);
		inUse = oldInuse;
	}
}

/*******************  FUNCTION  *********************/
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
	json.printField("stats",value.allocStats);
	json.closeStruct();
}

/*******************  FUNCTION  *********************/
void LocalAllocStackProfiler::solveSymbols(SymbolSolver& symbolResolver) const
{
	this->stackSizeAnalyser.solveSymbols(symbolResolver);
}

/*******************  FUNCTION  *********************/
Stack* LocalAllocStackProfiler::getStack(void )
{
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
}

/*******************  FUNCTION  *********************/
bool LocalAllocStackProfiler::isEnterExit(void)
{
	return stackMode == STACK_MODE_ENTER_EXIT_FUNC;
}

/*******************  FUNCTION  *********************/
FunctionStat::FunctionStat(void)
{
	this->count = 0;
	this->sum = 0;
	this->time = 0;
}

/*******************  FUNCTION  *********************/
void FunctionStat::inc(ssize_t value,ticks time)
{
	this->count++;
	this->sum += value;
	this->time += time;
}

/*******************  FUNCTION  *********************/
void convertToJson(htopml::JsonState& json, const FunctionStat& value)
{
	json.openStruct();
	json.printField("count",value.count);
	json.printField("sum",value.sum);
	json.printField("time",value.time);
	json.closeStruct();
}

/*******************  FUNCTION  *********************/
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

}
