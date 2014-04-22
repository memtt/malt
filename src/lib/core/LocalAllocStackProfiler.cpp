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
	this->globalProfiler = globalProfiler;
	this->options = globalProfiler->getOptions();
	this->stackMode = globalProfiler->getStackMode();
	enterExitStack.enterFunction((void*)0x1);
	backtraceStack.loadCurrentStack();
	
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
		inUse = oldInuse;
	}
}

/*******************  FUNCTION  *********************/
void LocalAllocStackProfiler::onEnterFunc(void* this_fn, void* call_site)
{
	//stack current loc tracking
	//TODO this is also done by LocalAllocStackProfiler, maybe try to point his object instead of recompute
	enterExitStack.enterFunction(call_site);
	
	//max stack
	if (options->maxStackEnabled)
		stackSizeAnalyser.onEnterFunc(this_fn);
}

/*******************  FUNCTION  *********************/
void LocalAllocStackProfiler::onExitFunc(void* this_fn, void* call_site)
{
	//stack current loc tracking
	//TODO this is also done by LocalAllocStackProfiler, maybe try to point his object instead of recompute
	enterExitStack.exitFunction(call_site);
	
	//max stack
	if (options->maxStackEnabled)
		stackSizeAnalyser.onExitFunc(this_fn);
}

/*******************  FUNCTION  *********************/
void convertToJson(htopml::JsonState& json, const LocalAllocStackProfiler& value)
{
	json.openStruct();
	json.printField("stackMem",value.stackSizeAnalyser);
	json.printField("cntMemOps",value.cntMemOps);
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
	}
}

}
