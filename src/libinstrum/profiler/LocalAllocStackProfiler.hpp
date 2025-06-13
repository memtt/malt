/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.6
*    DATE     : 06/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/profiler/LocalAllocStackProfiler.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (ECR) - 2014
*    AUTHOR   : Sébastien Valat - 2014
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

#ifndef MALT_LOCAL_ALLOC_STACK_PROFILER_HPP
#define MALT_LOCAL_ALLOC_STACK_PROFILER_HPP

/**********************************************************/
//std
#include <cstdlib>
//locals
#include "core/StackSizeTracker.hpp"
#include <stacks/EnterExitStack.hpp>
#include <stacks/BacktracePythonStack.hpp>
#include "AllocStackProfiler.hpp"
#include "core/StackSizeAnalyser.hpp"
#include "core/DomainCounters.hpp"

/**********************************************************/

/**********************************************************/
namespace MALT
{

/**********************************************************/
enum MallocKind
{
	MALLOC_KIND_MALLOC,
	MALLOC_KIND_POSIX_MEMALIGN,
	MALLOC_KIND_ALIGNED_ALLOC,
	MALLOC_KIND_MEMALIGN,
	MALLOC_KIND_VALLOC,
	MALLOC_KIND_PVALLOC,
	MALLOC_KIND_MAX,
};

/**********************************************************/
enum Language
{
	LANG_C,
	LANG_PYTHON
};

/**********************************************************/
struct FunctionStat
{
	FunctionStat(void);
	void inc(ssize_t size,ticks time);
	size_t count;
	ssize_t sum;
	ticks time;
};

/**********************************************************/
struct PerThreadAllocStats
{
	FunctionStat malloc[MALLOC_KIND_MAX];
	FunctionStat free;
	FunctionStat calloc;
	FunctionStat realloc;
};

/**********************************************************/
void convertToJson(htopml::JsonState& json, const FunctionStat& value);
void convertToJson(htopml::JsonState& json, const PerThreadAllocStats& value);

/**********************************************************/
/**
 * Object to be instntiate localy per thread to track stack and local parameters without loks.
 * It will cooperate with the global AllocStackProfiler to push the values on the global
 * tree when needed.
 * 
 * \b CAUTION, there is no locks here, this object must be pointed by TLS, not for global access.
**/
class LocalAllocStackProfiler
{
	public:
		LocalAllocStackProfiler(AllocStackProfiler * globalProfiler);
		~LocalAllocStackProfiler(void);
		void onMalloc(void* res, size_t size, ticks time, MALT::MallocKind kind, Language lang = LANG_C, AllocDomain domain = DOMAIN_C_ALLOC);
		void onFree(void* ptr, ticks time, Language lang = LANG_C);
		void onCalloc(void * res,size_t nmemb,size_t size, ticks time, Language lang = LANG_C, AllocDomain domain = DOMAIN_C_ALLOC);
		void onRealloc(void* ptr, void* res, size_t size, ticks time, Language lang = LANG_C, AllocDomain domain = DOMAIN_C_ALLOC);
		void onMmap(void * ptr, size_t size,int flags,int fd);
		void onMunmap(void * ptr, size_t size);
		inline void onEnterFunc(LangAddress this_fn,LangAddress call_site,bool ignoreStack=false);
		inline void onExitFunc(LangAddress this_fn,LangAddress call_site,bool ignoreStack=false);
		void solveSymbols(SymbolSolver & symbolResolver) const;
		bool isEnterExit(void);
		inline BacktracePythonStack getBacktracePythonStack(void);
		void loadPythonFirstBacktrace(void);
	public:
		friend void convertToJson(htopml::JsonState& json, const LocalAllocStackProfiler& value);
	protected:
		Stack * getStack(Language lang, size_t size, bool isFree);
	private:
		/** Pointer to the global state **/
		AllocStackProfiler * globalProfiler;
		/** Pointer to option object **/
		const Options * options;
		/** Object used to follow the local stack for the enter/exit mode. **/
		EnterExitStack enterExitStack;
		/** Object used to follow the local stack for the backtrace mode. **/
		BacktraceStack backtraceStack;
		/** Final mixed stack to cache faster. */
		Stack mixStack{STACK_ORDER_DESC};
		/** Object used to follow the local stack for backtrace mode in python */
		BacktracePythonStack backtracePythonStack;
		/** Follow size of stack in enter-exit mode. **/
		StackSizeAnalyser stackSizeAnalyser;
		/** Counter memory requests. **/
		size_t cntMemOps;
		/** Ways to track stack. **/
		StackMode stackMode;
		/** Pointer to the global enterExitStackTracer for enter-exit mode **/
		AllocTreeStrackTracer * enterExitStackTracer;
		AllocTreeStrackTracer::Handler enterExitHandler;
		PerThreadAllocStats allocStats;
		EnterExitStack noneStackC;
		EnterExitStack noneStackPython;
};

/**********************************************************/
inline void LocalAllocStackProfiler::onEnterFunc(LangAddress this_fn, LangAddress call_site, bool ignoreStack)
{
	//stack current loc tracking
	//TODO this is also done by LocalAllocStackProfiler, maybe try to point his object instead of recompute
	enterExitStack.enterFunction(call_site);
	
	//move into enter-exit tree
	//this->enterExitHandler = this->enterExitStackTracer->getChild(enterExitHandler,call_site);
	
	//max stack
	if (options->maxStackEnabled && !ignoreStack)
		stackSizeAnalyser.onEnterFunc(this_fn);
}

/**********************************************************/
inline void LocalAllocStackProfiler::onExitFunc(LangAddress this_fn, LangAddress call_site, bool ignoreStack)
{
	//stack current loc tracking
	//TODO this is also done by LocalAllocStackProfiler, maybe try to point his object instead of recompute
	enterExitStack.exitFunction(call_site);
	
	//move into enter-exit tree
	//this->enterExitHandler = this->enterExitStackTracer->getParent(enterExitHandler);
	
	//max stack
	if (options->maxStackEnabled && !ignoreStack)
		stackSizeAnalyser.onExitFunc(this_fn);
}

/**********************************************************/
BacktracePythonStack LocalAllocStackProfiler::getBacktracePythonStack(void)
{
	return this->backtracePythonStack;
}

}

#endif //MALT_LOCAL_ALLOC_STACK_PROFILER_HPP
