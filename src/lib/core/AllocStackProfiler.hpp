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
#include <valprof/ProfiledValue.hpp>
#include <tools/ELFReader.hpp>
#include "SegmentTracker.hpp"
#include "StackSizeTracker.hpp"
#include "SimpleStackTracer.hpp"
#include "AllocTracer.hpp"

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
	bool operator < (const ReallocJump & jump) const {return oldSize < jump.oldSize || (oldSize == jump.oldSize && newSize < jump.newSize);};
	size_t oldSize;
	size_t newSize;
};

/*********************  TYPES  **********************/
typedef std::list<LocalAllocStackProfiler *,STLInternalAllocator<LocalAllocStackProfiler*> > LocalAllocStackProfilerList;
typedef StackSTLHashMap<CallStackInfo> MMStackMap;
typedef std::map<size_t,size_t> AllocSizeDistrMap;
typedef std::map<ReallocJump,size_t> ReallocJumpMap;
typedef RLockFreeTree<CallStackInfo> AllocTreeStrackTracer;
typedef std::map<std::string,ElfGlobalVariableVector> GlobalVariableMap;

/*********************  STRUCT  *********************/
struct TimeTrackMemory
{
	//constructor & usefull funcs
	TimeTrackMemory();
	void set(const TimeTrackMemory & v);
	bool push(const TimeTrackMemory & v);
	friend void convertToJson(htopml::JsonState& json, const TimeTrackMemory & value);
	//data
	size_t requestedMem;
	size_t physicalMem;
	size_t virtualMem;
	size_t internalMem;
	size_t segments;
};

/*********************  STRUCT  *********************/
struct TimeTrackSysMemory
{
	//constructor & usefull funcs
	TimeTrackSysMemory();
	void set(const TimeTrackSysMemory & v);
	bool push(const TimeTrackSysMemory & v);
	friend void convertToJson(htopml::JsonState& json, const TimeTrackSysMemory & value);
	//data
	size_t freeMemory;
	size_t cachedMemory;
	size_t swapMemory;
};

/*********************  STRUCT  *********************/
struct TimeTrackAllocBandwidth
{
	size_t allocMem;
	size_t allocCount;
	size_t freeMem;
	size_t freeCount;
};

/*********************  CLASS  **********************/
class AllocStackProfiler
{
	public:
		AllocStackProfiler(const Options & option, StackMode mode = STACK_MODE_BACKTRACE,bool threadSafe = false);
		void onMalloc(void * ptr,size_t size,Stack * userStack = NULL);
		void onCalloc(void * ptr,size_t nmemb,size_t size,Stack * userStack = NULL);
		void onPrepareRealloc(void * oldPtr,Stack * userStack = NULL);
		void onMunmap(void * ptr,size_t size,Stack * userStack = NULL);
		void onLLMmap(void * ptr,size_t size,Stack * userStack = NULL);
		void onLLMunmap(void * ptr,size_t size,Stack * userStack = NULL);
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
		LocalAllocStackProfiler * createLocalStackProfiler(bool reentrant);
		void destroyLocalStackProfiler(LocalAllocStackProfiler * localProfiler);
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
		void loadGlobalVariables(void);
	private:
		//SimpleStackTracer stackTracer;
		StackSTLHashMap<CallStackInfo> stackTracer;
		RLockFreeTree<CallStackInfo> treeStackTracer;
// 		AbstractStackTree<CallStackInfo> stackTree;
		AllocSizeDistrMap sizeMap;
		SegmentTracker segTracker;
		ProfiledCumulValue allocBandwidth;
		ProfiledCumulValue freeBandwidth;
		ProfiledCumulValue allocCnt;
		ProfiledCumulValue freeCnt;
		GlobalVariableMap globalVariables;
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
		AllocTracer tracer;
		size_t osTotalMemory;
		size_t osFreeMemoryAtStart;
		size_t osCachedMemoryAtStart;
		size_t peakId;
		size_t peak;
		size_t curReq;
		std::string traceFilename;
		
		ProfiledValue<TimeTrackMemory> memoryTimeline;
		TimeTrackMemory curMemoryTimeline;
		ProfiledValue<TimeTrackSysMemory> systemTimeline;
		TimeTrackSysMemory curSystemTimeline;
};

}

#endif //MATT_ALLOC_STACK_PROGILER_HPP
