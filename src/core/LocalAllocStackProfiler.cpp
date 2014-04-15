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
	enterExitStack.enterFunction((void*)0x1);
	
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
		CODE_TIMING("mallocProf",globalProfiler->onMalloc(res,size,&enterExitStack));
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
		CODE_TIMING("freeProf",globalProfiler->onFree(ptr,&enterExitStack));
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
		CODE_TIMING("callocProf",globalProfiler->onCalloc(res,nmemb,size,&enterExitStack));
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
		CODE_TIMING("reallocProf",globalProfiler->onRealloc(ptr,res,size,&enterExitStack));
		this->cntMemOps++;
		inUse = oldInuse;
	}
}

/*******************  FUNCTION  *********************/
void LocalAllocStackProfiler::onEnterFunc(void* this_fn, void* call_site)
{
	//stack current loc tracking
	//TODO this is also done by LocalAllocStackProfiler, maybe try to point his object instead of recompute
	enterExitStack.enterFunction(this_fn);
	
	//max stack
	if (options->maxStackEnabled)
		stackSizeAnalyser.onEnterFunc(this_fn);
}

/*******************  FUNCTION  *********************/
void LocalAllocStackProfiler::onExitFunc(void* this_fn, void* call_site)
{
	//stack current loc tracking
	//TODO this is also done by LocalAllocStackProfiler, maybe try to point his object instead of recompute
	enterExitStack.exitFunction(this_fn);
	
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

}
