/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MATT_LOCAL_ALLOC_STACK_PROFILER_HPP
#define MATT_LOCAL_ALLOC_STACK_PROFILER_HPP

/********************  HEADERS  *********************/
//std
#include <cstdlib>
//locals
#include "StackSizeTracker.hpp"
#include "EnterExitCallStack.hpp"
#include "AllocStackProfiler.hpp"
#include "StackSizeAnalyser.hpp"

/*******************  FUNCTION  *********************/

/*******************  NAMESPACE  ********************/
namespace MATT
{

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
		void onMalloc(void * res,size_t size);
		void onFree(void * ptr);
		void onCalloc(void * res,size_t nmemb,size_t size);
		void onRealloc(void * ptr,void * res, size_t size);
		void onEnterFunc(void *this_fn,void *call_site);
		void onExitFunc(void *this_fn,void *call_site);
		void resolveSymbols(SymbolResolver & symbolResolver) const;
	public:
		friend void convertToJson(htopml::JsonState& json, const LocalAllocStackProfiler& value);
	private:
		/** Pointer to the global state **/
		AllocStackProfiler * globalProfiler;
		/** Pointer to option object **/
		const Options * options;
		/** Object used to follow the local stack for the enter/exit mode. **/
		EnterExitCallStack enterExitStack;
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
};

}

#endif //MATT_LOCAL_ALLOC_STACK_PROFILER_HPP
