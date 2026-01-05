/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 10/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/profiler/AllocStackProfiler.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (ECR) - 2014
*    AUTHOR   : Sébastien Valat - 2014 - 2019
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

#ifndef MALT_ALLOC_STACK_PROGILER_HPP
#define MALT_ALLOC_STACK_PROGILER_HPP

/**********************************************************/
//standard
#include <list>
#include <cstdlib>
#include <atomic>
//os specific (TODO move)
#include <sys/time.h>
#include <unistd.h>
//intenrals
#include <common/Options.hpp>
#include <common/RangePtr.hpp>
#include <portability/Mutex.hpp>
#include <stacks/EnterExitStack.hpp>
#include <stack-tree/StackSTLHashMap.hpp>
#include <stack-tree/RLockFreeTree.hpp>
#include <stack-tree/StackTreeCache.hpp>
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
#include <core/PythonSymbolTracker.hpp>
#include <core/Trigger.hpp>
#include <core/DomainCounters.hpp>
#include <stacks/MultiLangStackMerger.hpp>
//from v2
#include <stack-tree/from-v2/RLockFreeTree.hpp>
#include <stacks/StackReducer.hpp>

/**********************************************************/
namespace MALT
{

/**********************************************************/
class LocalAllocStackProfiler;

/**********************************************************/
struct ReallocJump
{
	bool operator < (const ReallocJump & jump) const {return oldSize < jump.oldSize || (oldSize == jump.oldSize && newSize < jump.newSize);};
	size_t oldSize;
	size_t newSize;
};

/**********************************************************/
typedef std::list<LocalAllocStackProfiler *,STLInternalAllocator<LocalAllocStackProfiler*> > LocalAllocStackProfilerList;
typedef StackSTLHashMap<CallStackInfo> MMStackMap;
typedef std::map<size_t,size_t, std::less<size_t>, STLInternalAllocator<std::pair<size_t, size_t> > > AllocSizeDistrMap;
typedef std::map<ReallocJump,size_t> ReallocJumpMap;
typedef RLockFreeTree<CallStackInfo> AllocTreeStrackTracer;
typedef std::map<std::string,ElfGlobalVariableVector> GlobalVariableMap;

/**********************************************************/
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

/**********************************************************/
struct FreeFinalInfos
{
	size_t size{0};
	ticks lifetime{0};
	const Stack * allocStack{nullptr};
};

/**********************************************************/
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

/**********************************************************/
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

/**********************************************************/
typedef void * (*MallocFuncPtr) (size_t size);

/**********************************************************/
class AllocStackProfiler
{
	public:
		AllocStackProfiler(const Options & option, StackMode mode = STACK_MODE_BACKTRACE,bool threadSafe = false);
		void onMalloc(AllocTracerEvent & traceEntry, void * ptr,size_t size,Stack * userStack = NULL, AllocDomain domain = DOMAIN_C_ALLOC);
		void onCalloc(AllocTracerEvent & traceEntry, void * ptr,size_t nmemb,size_t size,Stack * userStack = NULL, AllocDomain domain = DOMAIN_C_ALLOC);
		void onPrepareRealloc(void * oldPtr,Stack * userStack = NULL);
		void onMmap(AllocTracerEvent & traceEntry, void * ptr,size_t size,Stack * userStack = NULL, MMCallStackNode* callStackNode = NULL);
		void onMunmap(AllocTracerEvent & traceEntry, void * ptr,size_t size,Stack * userStack = NULL, MMCallStackNode* callStackNode = NULL);
		void onMremap(AllocTracerEvent & traceEntry, void * ptr,size_t size,void * new_ptr, size_t new_size, Stack * userStack = NULL);
		void onLLMmap(AllocTracerEvent & traceEntry, void * ptr,size_t size,Stack * userStack = NULL);
		void onLLMunmap(AllocTracerEvent & traceEntry, void * ptr,size_t size,Stack * userStack = NULL);
		size_t onRealloc(AllocTracerEvent & traceEntry, void* oldPtr, void* ptr, size_t newSize, MALT::Stack* userStack = 0, AllocDomain domain = DOMAIN_C_ALLOC);
		void onFree(AllocTracerEvent & traceEntry, void * ptr,Stack * userStack = NULL, AllocDomain domain = DOMAIN_C_ALLOC);
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
		LocalAllocStackProfiler * createLocalStackProfiler();
		void destroyLocalStackProfiler(LocalAllocStackProfiler * localProfiler);
		void setRealMallocAddr(MallocFuncPtr realMallocFunc);
		PythonSymbolTracker & getPythonSymbolTracker(void);
		MultiLangStackMerger & getMultiLangStackMerger(void) {return this->multiLangStackMerger;};
		void registerMaltJeMallocMem(ssize_t value);
		void onUpdateMem(const OSProcMemUsage & procMem, const OSMemUsage & sysMem);
		inline bool isAcceptedBySampling(size_t size, bool isFree);
	public:
		friend void convertToJson(htopml::JsonState& json, const AllocStackProfiler& value);
	private:
		MMCallStackNode getStackNode(MALT::Stack* userStack = 0);
		void onAllocEvent(void* ptr, size_t size, Stack* userStack, MMCallStackNode* callStackNode = NULL, bool doLock = true, AllocDomain domain = DOMAIN_C_ALLOC);
		FreeFinalInfos onFreeEvent(void* ptr, MALT::Stack* userStack, MALT::MMCallStackNode* callStackNode = 0, bool doLock = true, AllocDomain domain = DOMAIN_C_ALLOC, bool subMunmap = false);
		void solvePerThreadSymbols(void);
		void memOpsLevels(void);
		void updatePeakInfoOfStacks(void);
		bool peakTracking(ssize_t delta);
		void loadGlobalVariables(void);
		void loopSuppress(void);
		std::string getFileExeScriptName(void) const;
		void pythonImportSuppress(void);
		bool isImportStack(const Stack & stack) const;
		void applyVmaPatches(Stack* userStack, MMCallStackNode* callStackNode, VmaSegmentPatches & vmaPatches);
	private:
		//SimpleStackTracer stackTracer;
		MultiLangStackMerger multiLangStackMerger;
		StackSTLHashMap<CallStackInfo> stackTracker;
		StackTreeCache<MMCallStackNode> stackTrackerCache{4096};
		RLockFreeTree<CallStackInfo> treeStackTracker;
// 		AbstractStackTree<CallStackInfo> stackTree;
		AllocSizeDistrMap sizeMap;
		SegmentTracker segTracker;
		SegmentTracker mmapSegTracker;
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
		PythonSymbolTracker pythonSymbolTracker;
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
		size_t nbAllocSeen;
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
		bool skipThreadRegister{false};
		std::atomic<size_t> maltJeMallocMem{0};
		Trigger trigger;
		StackReducer reducer{10};
		DomainCounters domains;
		std::atomic<size_t> rate{0};
		std::atomic<size_t> rateCnt{0};
		std::atomic<size_t> nextThreadId{0};
};

/**********************************************************/
inline bool AllocStackProfiler::isAcceptedBySampling(size_t size, bool isFree)
{
	//trivial
	if (gblOptions->stackSampling == false)
		return true;

	//trivial
	if (isFree)
		return !gblOptions->stackSampling;

	//sum
	size_t previous = this->rate.fetch_add(size);
	size_t previousCnt = this->rateCnt.fetch_add(1);
	size_t bw = gblOptions->stackSamplingBw;
	if (previous / bw != (previous + size) / bw)
	{
		this->rate.fetch_sub(bw);
		return true;
	}

	//count
	size_t cnt = gblOptions->stackSamplingCnt;
	if (cnt != 0 && previousCnt >= cnt) {
		this->rateCnt.fetch_sub(cnt);
		return true;
	}

	return false;
}

std::string cmdToString(const OSCmdLine & cmdline);

}

#endif //MALT_ALLOC_STACK_PROGILER_HPP
