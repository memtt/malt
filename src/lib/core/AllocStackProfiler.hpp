/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MATT_ALLOC_STACK_PROGILER_HPP
#define MATT_ALLOC_STACK_PROGILER_HPP

/********************  HEADERS  *********************/
//standard
#include <list>
#include <cstdlib>
//os specific (TODO move)
#include <sys/time.h>
#include <unistd.h>
//intenrals
#include <common/Options.hpp>
#include <portability/Mutex.hpp>
#include "SegmentTracker.hpp"
#include "SimpleStackTracer.hpp"
#include <stacks/EnterExitStack.hpp>
#include "ProfiledValue.hpp"
#include "StackSizeTracker.hpp"
#include <stack-tree/StackSTLHashMap.hpp>

/*******************  NAMESPACE  ********************/
namespace MATT
{

/*********************  TYPES  **********************/
class LocalAllocStackProfiler;

/********************  ENUM  ************************/
enum StackMode
{
	STACK_MODE_BACKTRACE,
	STACK_MODE_ENTER_EXIT_FUNC,
	STACK_MODE_USER
};

/*********************  TYPES  **********************/
typedef std::list<LocalAllocStackProfiler *,STLInternalAllocator<LocalAllocStackProfiler*> > LocalAllocStackProfilerList;
typedef StackSTLHashMap<CallStackInfo> MMStackMap;

/*********************  CLASS  **********************/
class AllocStackProfiler
{
	public:
		AllocStackProfiler(const Options & option, StackMode mode = STACK_MODE_BACKTRACE,bool threadSafe = false);
		void onMalloc(void * ptr,size_t size,Stack * userStack = NULL);
		void onCalloc(void * ptr,size_t nmemb,size_t size,Stack * userStack = NULL);
		void onPrepareRealloc(void * oldPtr,Stack * userStack = NULL);
		void onRealloc(void * oldPtr,void * ptr,size_t newSize,Stack * userStack = NULL);
		void onFree(void * ptr,Stack * userStack = NULL);
		void onExit(void);
		void onEnterFunction(void * funcAddr);
		void onExitFunction(void * funcAddr);
		void onLargerStackSize(const StackSizeTracker & stackSizes,const Stack & stack);
		const Options * getOptions(void) const;
		void registerPerThreadProfiler(LocalAllocStackProfiler * profiler);
		ticks ticksPerSecond(void) const;
		StackMode getStackMode(void) const {return mode;};
	public:
		friend void convertToJson(htopml::JsonState& json, const AllocStackProfiler& value);
	private:
		MMCallStackNode getStackNode(MATT::Stack* userStack = 0);
		void onAllocEvent(void* ptr, size_t size, Stack* userStack, MMCallStackNode* callStackNode = NULL, bool doLock = true);
		void onFreeEvent(void* ptr, Stack* userStack, MMCallStackNode* callStackNode = NULL, bool doLock = true);
		void resolvePerThreadSymbols(void);
	private:
		//SimpleStackTracer stackTracer;
		StackSTLHashMap<CallStackInfo> stackTracer;
		SegmentTracker segTracker;
		ProfiledValue requestedMem;
		ProfiledValue physicalMem;
		ProfiledValue virtualMem;
		ProfiledValue internalMem;
		ProfiledValue segments;
		StackMode mode;
		Mutex lock;
		bool threadSafe;
		Options options;
		Mutex largestStackLock;
		unsigned long largestStackSize;
		StackSizeTracker largestStackMem;
		Stack largestStack;
		SymbolResolver symbolResolver;
		LocalAllocStackProfilerList perThreadProfiler;
		timeval trefSec;
		ticks trefTicks;
};

}

#endif //MATT_ALLOC_STACK_PROGILER_HPP
