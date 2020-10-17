/*****************************************************
             PROJECT  : MALT
             VERSION  : 1.2.2
             DATE     : 10/2020
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MALT_ALLOC_STACK_PROGILER_HPP
#define MALT_ALLOC_STACK_PROGILER_HPP

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
#include <valprof/Scatter2DValues.hpp>
#include <tools/ELFReader.hpp>
#include <core/SegmentTracker.hpp>
#include <core/StackSizeTracker.hpp>
#include <core/SimpleStackTracker.hpp>
#include <core/AllocTraceFile.hpp>
#include <core/VmaTracker.hpp>
//from v2
#include <stack-tree/from-v2/RLockFreeTree.hpp>

/*******************  NAMESPACE  ********************/
namespace MALT
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
	bool reduce(const TimeTrackMemory & v);
	friend void convertToJson(htopml::JsonState& json, const TimeTrackMemory & value);
	//data
	size_t requestedMem;
	size_t physicalMem;
	size_t virtualMem;
	size_t internalMem;
	size_t segments;
	//meta description
	static const char * selfDescribeFields[5];
};

/*********************  STRUCT  *********************/
struct TimeTrackSysMemory
{
	//constructor & usefull funcs
	TimeTrackSysMemory();
	bool reduce(const TimeTrackSysMemory & v);
	friend void convertToJson(htopml::JsonState& json, const TimeTrackSysMemory & value);
	//data
	size_t freeMemory;
	size_t cachedMemory;
	size_t swapMemory;
	//meta description
	static const char * selfDescribeFields[3];
};

/*********************  STRUCT  *********************/
struct TimeTrackAllocBandwidth
{
	TimeTrackAllocBandwidth();
	TimeTrackAllocBandwidth & operator += (const TimeTrackAllocBandwidth & value);
	friend void convertToJson(htopml::JsonState& json, const TimeTrackAllocBandwidth & value);
	bool reduce(const TimeTrackAllocBandwidth & v);
	//friend TimeTrackAllocBandwidth operator 
	size_t allocMem;
	size_t allocCount;
	size_t freeMem;
	size_t freeCount;
	//meta description
	static const char * selfDescribeFields[4];
};

/*********************  TYPE  ************************/
typedef void * (*MallocFuncPtr) (size_t size);

/*********************  CLASS  **********************/
class AllocStackProfiler
{
	public:
		AllocStackProfiler(const Options & option, StackMode mode = STACK_MODE_BACKTRACE,bool threadSafe = false);
		void onMalloc(void * ptr,size_t size,Stack * userStack = NULL);
		void onCalloc(void * ptr,size_t nmemb,size_t size,Stack * userStack = NULL);
		void onPrepareRealloc(void * oldPtr,Stack * userStack = NULL);
		void onMmap(void * ptr,size_t size,Stack * userStack = NULL);
		void onMunmap(void * ptr,size_t size,Stack * userStack = NULL);
		void onLLMmap(void * ptr,size_t size,Stack * userStack = NULL);
		void onLLMunmap(void * ptr,size_t size,Stack * userStack = NULL);
		size_t onRealloc(void* oldPtr, void* ptr, size_t newSize, MALT::Stack* userStack = 0);
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
		void setRealMallocAddr(MallocFuncPtr realMallocFunc);
	public:
		friend void convertToJson(htopml::JsonState& json, const AllocStackProfiler& value);
	private:
		MMCallStackNode getStackNode(MALT::Stack* userStack = 0);
		void onAllocEvent(void* ptr, size_t size, Stack* userStack, MMCallStackNode* callStackNode = NULL, bool doLock = true);
		  size_t onFreeEvent(void* ptr, MALT::Stack* userStack, MALT::MMCallStackNode* callStackNode = 0, bool doLock = true);
		void solvePerThreadSymbols(void);
		void memOpsLevels(void);
		void updatePeakInfoOfStacks(void);
		void peakTracking(ssize_t delta);
		void loadGlobalVariables(void);
		void loopSuppress(void);
	private:
		//SimpleStackTracer stackTracer;
		StackSTLHashMap<CallStackInfo> stackTracker;
		RLockFreeTree<CallStackInfo> treeStackTracker;
// 		AbstractStackTree<CallStackInfo> stackTree;
		AllocSizeDistrMap sizeMap;
		SegmentTracker segTracker;
		GlobalVariableMap globalVariables;
		StackMode mode;
		Spinlock lock;
		bool threadSafe;
		Options options;
		Spinlock largestStackLock;
		unsigned long largestStackSize;
		StackSizeTracker largestStackMem;
		Stack largestStack;
		SymbolSolver symbolResolver;
		LocalAllocStackProfilerList perThreadProfiler;
		timeval trefSec;
		ticks trefTicks;
		ticks sharedLinearIndex;
		ReallocJumpMap reallocJumpMap;
		AllocTraceFile tracer;
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
		ProfiledValue<TimeTrackAllocBandwidth> memoryBandwidth;
		VmaTracker vmaTracker;
		MALTV2::RLockFreeTree * stackTree;
		Scatter2DValues sizeOverTime;
		Scatter2DValues lifetimeOverSize;
		MallocFuncPtr realMallocAddr;
		std::string realMallocLib;
};

}

#endif //MALT_ALLOC_STACK_PROGILER_HPP
