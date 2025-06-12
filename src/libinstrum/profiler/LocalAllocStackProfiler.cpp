/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.6
*    DATE     : 06/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/profiler/LocalAllocStackProfiler.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (ECR) - 2014
*    AUTHOR   : Sébastien Valat - 2014
*    AUTHOR   : Bastien Levasseur - 2024
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
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
LocalAllocStackProfiler::LocalAllocStackProfiler(AllocStackProfiler* globalProfiler)
	:backtracePythonStack(globalProfiler->getPythonSymbolTracker())
{
	//errors
	assert(globalProfiler != NULL);
	
	//setup fields
	this->cntMemOps = 0;
	this->globalProfiler = globalProfiler;
	this->options = globalProfiler->getOptions();
	this->stackMode = globalProfiler->getStackMode();
	this->enterExitStackTracer = globalProfiler->getEnterExitStackTracer();
	enterExitStack.enterFunction(LangAddress(DOMAIN_C, (void*)0x1));
	backtraceStack.loadCurrentStack();

	this->noneStackC.enterFunction(LangAddress(DOMAIN_PYTHON, MALT_C_UNTRACKED_ID));
	this->noneStackPython.enterFunction(LangAddress(DOMAIN_PYTHON, MALT_PYTHON_UNTRACKED_ID));
	
	this->enterExitHandler = enterExitStackTracer->getRoot();
}

/**********************************************************/
LocalAllocStackProfiler::~LocalAllocStackProfiler(void)
{
	//nothing up to now
}

/**********************************************************/
//TODO: Pass Stack*, may be nullptr for C CallStack
//TODO: Pass Stack* to getStack()
void LocalAllocStackProfiler::onMalloc(void* res, size_t size, ticks time, MallocKind kind, Language lang, AllocDomain domain)
{
	//check for reentrance
	CODE_TIMING("mallocProf",globalProfiler->onMalloc(res,size,getStack(lang), domain));
	this->cntMemOps++;
	this->allocStats.malloc[kind].inc(size,time);
}

/**********************************************************/
void LocalAllocStackProfiler::onFree(void* ptr, ticks time, Language lang)
{
	//check for reentrance
	CODE_TIMING("freeProf",globalProfiler->onFree(ptr,getStack(lang)));
	this->cntMemOps++;
	this->allocStats.free.inc(0,time);
}

/**********************************************************/
void LocalAllocStackProfiler::onCalloc(void * res,size_t nmemb, size_t size, ticks time, Language lang, AllocDomain domain)
{
	//check for reentrance
	CODE_TIMING("callocProf",globalProfiler->onCalloc(res,nmemb,size,getStack(lang), domain));
	this->cntMemOps++;
	this->allocStats.calloc.inc(size,time);
}

/**********************************************************/
void LocalAllocStackProfiler::onRealloc(void* ptr, void* res, size_t size,ticks time, Language lang, AllocDomain domain)
{
	//check for reentrance
	CODE_TIMING("reallocProf",globalProfiler->onRealloc(ptr,res,size,getStack(lang), domain));
	this->cntMemOps++;
	this->allocStats.realloc.inc(size,time);
}

/**********************************************************/
void LocalAllocStackProfiler::onMmap(void* ptr, size_t size, int flags, int fd)
{
	//skip file and non anon
	if ((flags & (MAP_ANON|MAP_ANONYMOUS)) == 0 || fd != 0)
		return;
	
	//get stack
	Stack * stack = getStack();
	
	//check for renentrance
	CODE_TIMING("userMmapProf",globalProfiler->onMmap(ptr,size,stack));
	
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
void LocalAllocStackProfiler::loadPythonFirstBacktrace(void)
{
	CODE_TIMING("loadCurrentStack",backtracePythonStack.loadCurrentStack());
	for (size_t i = 0 ; i < backtracePythonStack.getSize() ; i++) {
		this->enterExitStack.enterFunction(backtracePythonStack[i]);
	}
}

/**********************************************************/
//TODO: getStack should receive a parameter iff it's a Python stack
Stack* LocalAllocStackProfiler::getStack(Language lang)
{
	//check
	assert (lang == LANG_C || lang == LANG_PYTHON);

	//vars
	bool oldInUse;

	//trivial
	if (lang == LANG_C && stackMode == STACK_MODE_NONE)
		return &noneStackC;
	if (lang == LANG_PYTHON && gblOptions->pythonStackEnum == STACK_MODE_NONE)
		return &noneStackPython;

	//backtrance in C
	Stack * cRef = &this->enterExitStack;
	if(stackMode == STACK_MODE_BACKTRACE && (lang == LANG_C || gblOptions->pythonMix)) {
		CODE_TIMING("loadCurrentStack",backtraceStack.loadCurrentStack());
		backtraceStack.fastSkip(gblOptions->stackSkip);
		cRef = &backtraceStack;
	}

	//backtrace in python
	Stack * pythonRef = nullptr;
	if (gblOptions->pythonStackEnum == STACK_MODE_BACKTRACE && MALT::Py_IsInitialized()) {
		CODE_TIMING("loadCurrentStack",backtracePythonStack.loadCurrentStack());
		pythonRef = &backtracePythonStack;
	} else {
		//oldInUse = this->markInUseAndGetOldStatus();
		//CODE_TIMING("trackPythonStack", this->globalProfiler->getPythonSymbolTracker().makeStackPythonDomain(enterExitStack));
		//this->restoreInUseStatus(oldInUse);
		pythonRef = &this->enterExitStack;
	}

	//if backtrace in python is needed
	if (gblOptions->pythonMix && gblOptions->pythonInstru && lang != LANG_PYTHON) {
		//backtrace python to get last level (not captured by enter exit)
		LangAddress currentPythonAddr;
		if (pythonRef == &this->enterExitStack) {
			currentPythonAddr = backtracePythonStack.getCurrentFrameAddr();
			this->enterExitStack.enterFunction(currentPythonAddr);
		}

		if (cRef == pythonRef) {
			assert(cRef == &this->enterExitStack);
			globalProfiler->getMultiLangStackMerger().removePythonLib(mixStack, *cRef);
		} else {
			globalProfiler->getMultiLangStackMerger().mixPythonAndCStack(mixStack, *cRef, *pythonRef);
		}

		//exit last level we just added
		if (pythonRef == &this->enterExitStack) {
			this->enterExitStack.exitFunction(currentPythonAddr);
		}
		return &mixStack;
	} else {
		if (lang == LANG_PYTHON && gblOptions->pythonInstru)
			return pythonRef;
		else if (lang == LANG_C || gblOptions->pythonInstru == false)
			return cRef;
		else {
			MALT_FATAL("Invalid language, not supported !");
			return nullptr;
		}
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

}
