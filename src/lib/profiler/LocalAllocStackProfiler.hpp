/*****************************************************
             PROJECT  : MALT
             VERSION  : 1.2.2
             DATE     : 10/2020
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MALT_LOCAL_ALLOC_STACK_PROFILER_HPP
#define MALT_LOCAL_ALLOC_STACK_PROFILER_HPP

/********************  HEADERS  *********************/
//std
#include <cstdlib>
//locals
#include "core/StackSizeTracker.hpp"
#include <stacks/EnterExitStack.hpp>
#include "AllocStackProfiler.hpp"
#include "core/StackSizeAnalyser.hpp"

/*******************  FUNCTION  *********************/

/*******************  NAMESPACE  ********************/
namespace MALT
{

/*********************  ENUM  ***********************/
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

/********************  STRUCT  **********************/
struct FunctionStat
{
	FunctionStat(void);
	void inc(ssize_t size,ticks time);
	size_t count;
	ssize_t sum;
	ticks time;
};

/********************  STRUCT  **********************/
struct PerThreadAllocStats
{
	FunctionStat malloc[MALLOC_KIND_MAX];
	FunctionStat free;
	FunctionStat calloc;
	FunctionStat realloc;
};

/*******************  FUNCTION  *********************/
void convertToJson(htopml::JsonState& json, const FunctionStat& value);
void convertToJson(htopml::JsonState& json, const PerThreadAllocStats& value);

/*********************  CLASS  **********************/
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
		LocalAllocStackProfiler(AllocStackProfiler * globalProfiler,bool reentrance = true);
		~LocalAllocStackProfiler(void);
		void onMalloc(void* res, size_t size, ticks time, MALT::MallocKind kind);
		void onFree(void* ptr, ticks time);
		void onCalloc(void * res,size_t nmemb,size_t size, ticks time);
		void onRealloc(void* ptr, void* res, size_t size, ticks time);
		void onMmap(void * ptr, size_t size,int flags,int fd);
		void onMunmap(void * ptr, size_t size);
		inline void onEnterFunc(void *this_fn,void *call_site,bool ignoreStack=false);
		inline void onExitFunc(void *this_fn,void *call_site,bool ignoreStack=false);
		void solveSymbols(SymbolSolver & symbolResolver) const;
		bool isEnterExit(void);
	public:
		friend void convertToJson(htopml::JsonState& json, const LocalAllocStackProfiler& value);
	protected:
		Stack * getStack(void);
	private:
		/** Pointer to the global state **/
		AllocStackProfiler * globalProfiler;
		/** Pointer to option object **/
		const Options * options;
		/** Object used to follow the local stack for the enter/exit mode. **/
		EnterExitStack enterExitStack;
		/** Object used to follow the local stack for the backtrace mode. **/
		BacktraceStack backtraceStack;
		/** Follow size of stack in enter-exit mode. **/
		StackSizeAnalyser stackSizeAnalyser;
		/** Counter memory requests. **/
		size_t cntMemOps;
		/** Request for the object to take care of reentrance (disable not analyse inner calls). **/
		bool reentrance;
		/**
		 * Avoid to instrument inner allocations, marked at 'true' when entering in first level malloc/free.... 
		 * It permit to use dynamic allocation inside instrumentation functions.
		**/
		volatile bool inUse;
		/** Ways to track stack. **/
		StackMode stackMode;
		/** Pointer to the global enterExitStackTracer for enter-exit mode **/
		AllocTreeStrackTracer * enterExitStackTracer;
		AllocTreeStrackTracer::Handler enterExitHandler;
		PerThreadAllocStats allocStats;
};

/*******************  FUNCTION  *********************/
inline void LocalAllocStackProfiler::onEnterFunc(void* this_fn, void* call_site, bool ignoreStack)
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

/*******************  FUNCTION  *********************/
inline void LocalAllocStackProfiler::onExitFunc(void* this_fn, void* call_site, bool ignoreStack)
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

}

#endif //MALT_LOCAL_ALLOC_STACK_PROFILER_HPP
