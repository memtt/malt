/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 01/2026
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/profiler/LocalAllocStackProfiler.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (ECR) - 2014
*    AUTHOR   : Sébastien Valat - 2014 - 2026
*    AUTHOR   : Bastien Levasseur - 2024
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
//locals from common/
#include <common/Debug.hpp>
#include <common/CodeTiming.hpp>
#include <portability/Clock.hpp>
//Unix for mmap flags (need to move to portability directory)
#include <sys/mman.h>
//locals
#include "LocalAllocStackProfiler.hpp"

/**********************************************************/
namespace MALT
{

/**********************************************************/
LocalAllocStackProfiler::LocalAllocStackProfiler(AllocStackProfiler* globalProfiler, size_t threadId)
	:backtracePythonStack(globalProfiler->getPythonSymbolTracker())
{
	//errors
	assert(globalProfiler != NULL);
	
	//setup fields
	this->cntMemOps = 0;
	this->threadId = threadId;
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
AllocTraceEventType LocalAllocStackProfiler::allocKindToTraceType(MallocKind kind, AllocDomain domain)
{
	switch (kind) {
		case MALLOC_KIND_MALLOC: {
			switch (domain) {
				case DOMAIN_C_ALLOC:
					return EVENT_C_MALLOC;
				case DOMAIN_PYTHON_OBJ:
					return EVENT_PY_OBJ_MALLOC;
				case DOMAIN_PYTHON_MEM:
					return EVENT_PY_MEM_MALLOC;
				case DOMAIN_PYTHON_RAW:
					return EVENT_PY_RAW_MALLOC;
				case DOMAIN_GPU_ALLOC:
					return EVENT_GPU_MALLOC;
				default:
					MALT_FATAL("Should not reach this line !");
					return EVENT_NOP;
			}
		};
		case MALLOC_KIND_POSIX_MEMALIGN:
			return EVENT_C_POSIX_MEMALIGN;
		case MALLOC_KIND_ALIGNED_ALLOC:
			return EVENT_C_ALIGNED_ALLOC;
		case MALLOC_KIND_MEMALIGN:
			return EVENT_C_MEMALIGN;
		case MALLOC_KIND_VALLOC:
			return EVENT_C_VALLOC;
		case MALLOC_KIND_PVALLOC:
			return EVENT_C_PVALLOC;
		default:
			MALT_FATAL("Should never reach this line !");
			return EVENT_NOP;
	}
}

/**********************************************************/
//TODO: Pass Stack*, may be nullptr for C CallStack
//TODO: Pass Stack* to getStack()
void LocalAllocStackProfiler::onMalloc(void* res, size_t size, ticks time, MallocKind kind, Language lang, AllocDomain domain)
{
	//build trace entry
	AllocTracerEvent traceEntry;
	traceEntry.type = allocKindToTraceType(kind, domain);
	traceEntry.threadId = this->threadId;
	traceEntry.callStack = nullptr; //not yet known at this stage
	traceEntry.time = Clock::getticks();
	traceEntry.cost = time;
	traceEntry.addr = res;
	traceEntry.size = size;
	traceEntry.extra.generic.extra1 = 0;
	traceEntry.extra.generic.extra2 = 0;

	//check for reentrance
	CODE_TIMING("mallocProf",globalProfiler->onMalloc(traceEntry, res,size,getStack(lang, size, false, false), domain));
	this->popEnterExit();
	this->cntMemOps++;
	this->allocStats.malloc[kind].inc(size,time);
}

/**********************************************************/
void LocalAllocStackProfiler::onFree(void* ptr, ticks time, Language lang, AllocDomain domain)
{
	//build trace entry
	AllocTracerEvent traceEntry;
	switch (domain) {
		case DOMAIN_C_ALLOC:
			traceEntry.type = EVENT_C_FREE;
			break;
		case DOMAIN_PYTHON_OBJ:
			traceEntry.type = EVENT_PY_OBJ_FREE;
			break;
		case DOMAIN_PYTHON_MEM:
			traceEntry.type = EVENT_PY_MEM_FREE;
			break;
		case DOMAIN_PYTHON_RAW:
			traceEntry.type = EVENT_PY_RAW_FREE;
			break;
		case DOMAIN_GPU_ALLOC:
			traceEntry.type = EVENT_GPU_FREE;
			break;
		default:
			MALT_FATAL("Should not reach this line !");
			traceEntry.type = EVENT_NOP;
			break;
	}
	traceEntry.threadId = this->threadId;
	traceEntry.callStack = nullptr; //not yet known at this stage
	traceEntry.time = Clock::getticks();
	traceEntry.cost = time;
	traceEntry.addr = ptr;
	traceEntry.size = 0;
	traceEntry.extra.free.lifetime = 0;
	traceEntry.extra.free.allocStack = 0;

	//check for reentrance
	CODE_TIMING("freeProf",globalProfiler->onFree(traceEntry, ptr,getStack(lang, 0, true, false), domain));
	this->popEnterExit();
	this->cntMemOps++;
	this->allocStats.free.inc(0,time);
}

/**********************************************************/
void LocalAllocStackProfiler::onCalloc(void * res,size_t nmemb, size_t size, ticks time, Language lang, AllocDomain domain)
{
	//build trace entry
	AllocTracerEvent traceEntry;
	switch (domain) {
		case DOMAIN_C_ALLOC:
			traceEntry.type = EVENT_C_CALLOC;
			break;
		case DOMAIN_PYTHON_OBJ:
			traceEntry.type = EVENT_PY_OBJ_CALLOC;
			break;
		case DOMAIN_PYTHON_MEM:
			traceEntry.type = EVENT_PY_MEM_CALLOC;
			break;
		case DOMAIN_PYTHON_RAW:
			traceEntry.type = EVENT_PY_RAW_CALLOC;
			break;
		default:
			MALT_FATAL("Should not reach this line !");
			traceEntry.type = EVENT_NOP;
			break;
	}
	traceEntry.threadId = this->threadId;
	traceEntry.callStack = nullptr; //not yet known at this stage
	traceEntry.time = Clock::getticks();
	traceEntry.cost = time;
	traceEntry.addr = res;
	traceEntry.size = size;
	traceEntry.extra.generic.extra1 = 0;
	traceEntry.extra.generic.extra2 = 0;

	//check for reentrance
	const size_t totalSize = nmemb * size;
	CODE_TIMING("callocProf",globalProfiler->onCalloc(traceEntry, res,nmemb,size,getStack(lang, totalSize, false, false), domain));
	this->popEnterExit();
	this->cntMemOps++;
	this->allocStats.calloc.inc(totalSize,time);
}

/**********************************************************/
void LocalAllocStackProfiler::onRealloc(void* ptr, void* res, size_t size,ticks time, Language lang, AllocDomain domain)
{
	//build trace entry
	AllocTracerEvent traceEntry;
	switch (domain) {
		case DOMAIN_C_ALLOC:
			traceEntry.type = EVENT_C_REALLOC;
			break;
		case DOMAIN_PYTHON_OBJ:
			traceEntry.type = EVENT_PY_OBJ_REALLOC;
			break;
		case DOMAIN_PYTHON_MEM:
			traceEntry.type = EVENT_PY_MEM_REALLOC;
			break;
		case DOMAIN_PYTHON_RAW:
			traceEntry.type = EVENT_PY_RAW_REALLOC;
			break;
		default:
			MALT_FATAL("Should not reach this line !");
			traceEntry.type = EVENT_NOP;
			break;
	}
	traceEntry.threadId = this->threadId;
	traceEntry.callStack = nullptr; //not yet known at this stage
	traceEntry.time = Clock::getticks();
	traceEntry.cost = time;
	traceEntry.addr = res;
	traceEntry.size = size;
	traceEntry.extra.realloc.oldAddr = ptr;
	traceEntry.extra.realloc.oldSize = 0;

	//check for reentrance
	CODE_TIMING("reallocProf",globalProfiler->onRealloc(traceEntry, ptr,res,size,getStack(lang, size, false, false), domain));
	this->popEnterExit();
	this->cntMemOps++;
	this->allocStats.realloc.inc(size,time);
}

/**********************************************************/
void LocalAllocStackProfiler::onMmap(void* ptr, size_t size, int flags, int fd, ticks time)
{
	//skip file and non anon
	//if ((flags & (MAP_ANON|MAP_ANONYMOUS)) == 0)
	//	return;
	
	//get stack
	Stack * stack = getStack(LANG_C, size, false, true);

	//build trace entry
	AllocTracerEvent traceEntry;
	traceEntry.type = EVENT_C_MMAP;
	traceEntry.threadId = this->threadId;
	traceEntry.callStack = nullptr; //not yet known at this stage
	traceEntry.time = Clock::getticks();
	traceEntry.cost = time;
	traceEntry.addr = ptr;
	traceEntry.size = size;
	traceEntry.extra.generic.extra1 = 0;
	traceEntry.extra.generic.extra2 = 0;
	
	//check for renentrance
	CODE_TIMING("userMmapProf",globalProfiler->onMmap(traceEntry,ptr,size,stack));

	this->popEnterExit();

	this->cntMemOps++;
	this->allocStats.mmap.inc(size,time);
	
	//CODE_TIMING("globalMmapProf",globalProfiler->onGlobalMmap(ptr,size,stack));
}

/**********************************************************/
void LocalAllocStackProfiler::onMunmap(void* ptr, size_t size, ticks time)
{
	//get stack
	Stack * stack = getStack(LANG_C, size, false, true);

	//build trace entry
	AllocTracerEvent traceEntry;
	traceEntry.type = EVENT_C_MUNMAP;
	traceEntry.threadId = this->threadId;
	traceEntry.callStack = nullptr; //not yet known at this stage
	traceEntry.time = Clock::getticks();
	traceEntry.cost = time;
	traceEntry.addr = ptr;
	traceEntry.size = size;
	traceEntry.extra.generic.extra1 = 0;
	traceEntry.extra.generic.extra2 = 0;
	
	//check for renentrance
	CODE_TIMING("userMmapProf",globalProfiler->onMunmap(traceEntry,ptr,size,stack));

	this->popEnterExit();

	this->cntMemOps++;
	this->allocStats.munmap.inc(size,time);
}

/**********************************************************/
void LocalAllocStackProfiler::onMremap(void * ptr, size_t size, void * new_ptr, size_t new_size, ticks time)
{
	//get stack
	Stack * stack = getStack(LANG_C, size, false, true);

	//build trace entry
	AllocTracerEvent traceEntry;
	traceEntry.type = EVENT_C_MREMAP;
	traceEntry.threadId = this->threadId;
	traceEntry.callStack = nullptr; //not yet known at this stage
	traceEntry.time = Clock::getticks();
	traceEntry.cost = time;
	traceEntry.addr = ptr;
	traceEntry.size = size;
	traceEntry.extra.mremap.newAddr = new_ptr;
	traceEntry.extra.mremap.newSize = new_size;
	
	//check for renentrance
	CODE_TIMING("userMmapProf",globalProfiler->onMremap(traceEntry, ptr,size,new_ptr,new_size,stack));

	this->popEnterExit();

	this->cntMemOps++;
	this->allocStats.mremap.inc(size,time);
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
	for (int i = 0 ; i < backtracePythonStack.getSize() ; i++) {
		this->enterExitStack.enterFunction(backtracePythonStack[i]);
	}
}

/**********************************************************/
//TODO: getStack should receive a parameter iff it's a Python stack
Stack* LocalAllocStackProfiler::getStack(Language lang, size_t size, bool isFree, bool isMmmap)
{
	//check
	assert (lang == LANG_C || lang == LANG_PYTHON);

	//copy localy
	StackMode localCStackMode = this->stackMode;
	StackMode localPyStackMode = this->options->python.stack;

	//apply tricks
	//currently has an issue if python trigger an internal mmap call for its own handling
	//because then we cannot walk the stack due to GIL locking and re-entrance issue.
	if (isMmmap) {
		if (localCStackMode == StackMode::STACK_MODE_NONE && localPyStackMode != StackMode::STACK_MODE_NONE)
			localCStackMode = StackMode::STACK_MODE_BACKTRACE;
		else if (localCStackMode == StackMode::STACK_MODE_PYTHON && localPyStackMode == StackMode::STACK_MODE_ENTER_EXIT_FUNC)
			localCStackMode = StackMode::STACK_MODE_BACKTRACE;//StackMode::STACK_MODE_ENTER_EXIT_FUNC;
		else if (localCStackMode == StackMode::STACK_MODE_PYTHON && localPyStackMode != StackMode::STACK_MODE_NONE)
			localCStackMode = StackMode::STACK_MODE_BACKTRACE;
		localPyStackMode = StackMode::STACK_MODE_NONE;
	}

	//check sampling
	bool rejectedBySampling = (this->globalProfiler->isAcceptedBySampling(size, isFree) == false);

	//return prev
	if (rejectedBySampling && this->samplePrev.getSize() > 0)
		return &this->samplePrev;

	//vars
	bool hasPythonGIL = false;
	PyGILState_STATE pythonGilState;
	Stack* result = nullptr;

	//trivial
	if (lang == LANG_C && (localCStackMode == STACK_MODE_NONE || rejectedBySampling)) {
		if (gblOptions->sampling.enabled)
			this->samplePrev.set(noneStackC);
		return &noneStackC;
	}
	if (lang == LANG_PYTHON && (localPyStackMode == STACK_MODE_NONE || rejectedBySampling)) {
		if (gblOptions->sampling.enabled)
			this->samplePrev.set(noneStackPython);
		return &noneStackPython;
	}

	//backtrance in C
	Stack * cRef = &this->enterExitStack;
	if(localCStackMode == STACK_MODE_BACKTRACE && (lang == LANG_C || gblOptions->python.mix)) {
		CODE_TIMING("loadCurrentStack",backtraceStack.loadCurrentStack());
		backtraceStack.fastSkip(gblOptions->stack.skip);
		cRef = &backtraceStack;
	}

	//backtrace in python
	Stack * pythonRef = nullptr;
	if (localPyStackMode == STACK_MODE_NONE) {
		pythonRef = cRef;
	} else if (localPyStackMode == STACK_MODE_BACKTRACE && MALT::Py_IsInitialized()) {
		pythonGilState = MALT::PyGILState_Ensure();
		hasPythonGIL = true;
		CODE_TIMING("loadCurrentStackPy",backtracePythonStack.loadCurrentStack());
		pythonRef = &backtracePythonStack;
	} else {
		//oldInUse = this->markInUseAndGetOldStatus();
		//CODE_TIMING("trackPythonStack", this->globalProfiler->getPythonSymbolTracker().makeStackPythonDomain(enterExitStack));
		//this->restoreInUseStatus(oldInUse);
		pythonRef = &this->enterExitStack;
	}

	//replace
	if (lang == LANG_C && localCStackMode == STACK_MODE_PYTHON)
		cRef = pythonRef;

	//get last
	//backtrace python to get last level (not captured by enter exit)
	if (lang != LANG_PYTHON && pythonRef == &this->enterExitStack && MALT::Py_IsInitialized() && isMmmap == false){
		if (pythonRef->getCallee().getDomain() == DOMAIN_PYTHON && hasPythonGIL == false) {
			pythonGilState = MALT::PyGILState_Ensure();
			hasPythonGIL = true;
		}
		CODE_TIMING("loadCurrentFramePy",currentPythonAddr = backtracePythonStack.getCurrentFrameAddr());
		this->enterExitStack.enterFunction(currentPythonAddr);
		assert(this->needToPop == false);
		this->needToPop = true;
	}

	//if backtrace in python is needed
	if (gblOptions->python.mix && gblOptions->python.enabled && lang != LANG_PYTHON) {
		if (cRef == pythonRef) {
			assert(cRef == &this->enterExitStack);
			CODE_TIMING("mixLang",globalProfiler->getMultiLangStackMerger().removePythonLib(mixStack, *cRef));
		} else {
			CODE_TIMING("mixLang",globalProfiler->getMultiLangStackMerger().mixPythonAndCStack(mixStack, *cRef, *pythonRef));
		}

		result = &mixStack;
	} else {
		if (lang == LANG_PYTHON && gblOptions->python.enabled)
			result = pythonRef;
		else if (lang == LANG_C || gblOptions->python.enabled == false)
			result = cRef;
		else {
			MALT_FATAL("Invalid language, not supported !");
			result = nullptr;
		}
	}	

	if (hasPythonGIL)
		MALT::PyGILState_Release(pythonGilState);

	if (gblOptions->sampling.enabled)
		this->samplePrev.set(*result);

	//ok
	return result;
}

/**********************************************************/
void LocalAllocStackProfiler::popEnterExit(void)
{
	//exit last level we just added
	if (this->needToPop) {
		this->enterExitStack.exitFunction(currentPythonAddr);
		this->needToPop = false;
	}
}

/**********************************************************/
bool LocalAllocStackProfiler::isEnterExit(void)
{
	return stackMode == STACK_MODE_ENTER_EXIT_FUNC;
}

/**********************************************************/
void LocalAllocStackProfiler::flushPythonCacheSolver(void)
{
	this->backtracePythonStack.flushLineCache();
}

/**********************************************************/
void LocalAllocStackProfiler::printStats(void) const
{
	#ifdef MALT_ENABLE_CODE_TIMING
		this->backtracePythonStack.displayCacheStats();
	#endif //MALT_ENABLE_CODE_TIMING
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
