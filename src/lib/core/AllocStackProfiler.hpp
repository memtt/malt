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
#include <stacks/EnterExitStack.hpp>
#include <stack-tree/StackSTLHashMap.hpp>
#include <stack-tree/RLockFreeTree.hpp>
// #include <stack-tree/AbstractStackTree.hpp>
#include <valprof/ProfiledStateValue.hpp>
#include <valprof/ProfiledCumulValue.hpp>
#include "SegmentTracker.hpp"
#include "StackSizeTracker.hpp"
#include "SimpleStackTracer.hpp"

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

/********************  STRUCT  **********************/
struct ReallocJump
{
	bool operator < (const ReallocJump & jump) const {return jump.oldSize < jump.oldSize || (jump.oldSize == jump.oldSize && jump.newSize == jump.newSize);};
	size_t oldSize;
	size_t newSize;
};

/*********************  TYPES  **********************/
typedef std::list<LocalAllocStackProfiler *,STLInternalAllocator<LocalAllocStackProfiler*> > LocalAllocStackProfilerList;
typedef StackSTLHashMap<CallStackInfo> MMStackMap;
typedef std::map<size_t,size_t> AllocSizeDistrMap;
typedef std::map<ReallocJump,size_t> ReallocJumpMap;
typedef RLockFreeTree<CallStackInfo> AllocTreeStrackTracer;

/*********************  CLASS  **********************/
class AllocStackProfiler
{
	public:
		AllocStackProfiler(const Options & option, StackMode mode = STACK_MODE_BACKTRACE,bool threadSafe = false);
		void onMalloc(void * ptr,size_t size,Stack * userStack = NULL);
		void onCalloc(void * ptr,size_t nmemb,size_t size,Stack * userStack = NULL);
		void onPrepareRealloc(void * oldPtr,Stack * userStack = NULL);
		size_t onRealloc(void* oldPtr, void* ptr, size_t newSize, MATT::Stack* userStack = 0);
		void onFree(void * ptr,Stack * userStack = NULL);
		void onExit(void);
		void onEnterFunction(void * funcAddr);
		void onExitFunction(void * funcAddr);
		void onLargerStackSize(const StackSizeTracker & stackSizes,const Stack & stack);
		const Options * getOptions(void) const;
		void registerPerThreadProfiler(LocalAllocStackProfiler * profiler);
		ticks ticksPerSecond(void) const;
		StackMode getStackMode(void) const {return mode;};
		void registerMaqaoFunctionSymbol(int funcId,const char * funcName,const char * file,int line);
		AllocTreeStrackTracer * getEnterExitStackTracer(void);
		bool isEnterExit(void);
	public:
		friend void convertToJson(htopml::JsonState& json, const AllocStackProfiler& value);
	private:
		MMCallStackNode getStackNode(MATT::Stack* userStack = 0);
		void onAllocEvent(void* ptr, size_t size, Stack* userStack, MMCallStackNode* callStackNode = NULL, bool doLock = true);
		  size_t onFreeEvent(void* ptr, MATT::Stack* userStack, MATT::MMCallStackNode* callStackNode = 0, bool doLock = true);
		void resolvePerThreadSymbols(void);
		void memOpsLevels(void);
		void updatePeakInfoOfStacks(void);
		void peakTracking(ssize_t delta);
	private:
		//SimpleStackTracer stackTracer;
		StackSTLHashMap<CallStackInfo> stackTracer;
		RLockFreeTree<CallStackInfo> treeStackTracer;
// 		AbstractStackTree<CallStackInfo> stackTree;
		AllocSizeDistrMap sizeMap;
		SegmentTracker segTracker;
		ProfiledStateValue requestedMem;
		ProfiledStateValue physicalMem;
		ProfiledStateValue virtualMem;
		ProfiledStateValue internalMem;
		ProfiledStateValue segments;
		ProfiledCumulValue allocBandwidth;
		ProfiledCumulValue freeBandwidth;
		StackMode mode;
		Spinlock lock;
		bool threadSafe;
		Options options;
		Spinlock largestStackLock;
		unsigned long largestStackSize;
		StackSizeTracker largestStackMem;
		Stack largestStack;
		SymbolResolver symbolResolver;
		LocalAllocStackProfilerList perThreadProfiler;
		timeval trefSec;
		ticks trefTicks;
		ticks sharedLinearIndex;
		ReallocJumpMap reallocJumpMap;
		size_t osTotalMemory;
		size_t osFreeMemoryAtStart;
		size_t osCachedMemoryAtStart;
		size_t peakId;
		size_t peak;
		size_t curReq;
};

}

#endif //MATT_ALLOC_STACK_PROGILER_HPP
