/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 02/2015
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MATT_PROCESS_LEVEL_ANLYSIS_HPP
#define MATT_PROCESS_LEVEL_ANLYSIS_HPP

/********************  HEADERS  *********************/
//std C++
#include <vector>
//internal
#include <core/Clock.hpp>
#include <hooks/ExitHooks.hpp>
#include <hooks/MmapHooks.hpp>
#include <hooks/MallocHooks.hpp>
#include <hooks/ThreadHooks.hpp>
#include <core/UserSegmentTracker.hpp>
#include <core/PeakTracker.hpp>
#include <core/TimeProfiler.hpp>
#include <core/SymbolRegistry.hpp>
#include <core/GlobalVariables.hpp>
#include <core/AllocTracer.hpp>
#include <core/AllocSizeDistribution.hpp>
#include <hooks/EnterExitFunctionHooks.hpp>
#include <allocators/STLInternalAllocatorNoFree.hpp>
//current
#include "ThreadLevelAnalysis.hpp"

namespace MATT
{

/*********************  TYPES  **********************/
typedef std::vector<ThreadLevelAnalysis*,STLInternalAllocatorNoFree<ThreadLevelAnalysis*> > ThreadLevelAnalysisVector;

/********************  ENUM  ************************/
enum ProcessAnalysisIDS
{
	MATT_ANA_ID_ALLOC = 0,
	MATT_ANA_ID_FREE = 1,
	MATT_ANA_ID_LIFETIME = 2,
	MATT_ANA_ID_PEAK = 3,
};

/********************  ENUM  ************************/
enum AllocMemStats
{
	MATT_MEM_STATS_REQUESTED,
	MATT_MEM_STATS_PHYSICAL,
	MATT_MEM_STATS_VIRTUAL,
	MATT_MEM_STATS_INTERNAL,
	MATT_MEM_STATS_SEGMENTS,
	//must stay on last position
	MATT_MEM_STATS_COUNT_ENTRIES
};

/********************  ENUM  ************************/
enum SystemStats
{
	MATT_SYS_STATS_FREE,
	MATT_SYS_STATS_CACHE,
	MATT_SYS_STATS_SWAP,
	//must stay on last position
	MATT_SYS_STATS_COUNT_ENTRIES
};

/********************  ENUM  ************************/
enum AllocOpsBandwidth
{
	MATT_OPS_BW_ALLOCATED_SIZE,
	MATT_OPS_BW_ALLOCATED_COUNT,
	MATT_OPS_BW_FREED_SIZE,
	MATT_OPS_BW_FREED_COUNT,
	//must stay on last position
	MATT_OPS_BW_COUNT_ENTRIES
};

/*********************  STRUCT  *********************/
struct AllocStateStats
{
	size_t requestedMem;
	size_t segmentCount;
};

/*********************  CLASS  **********************/
class ProcessLevelAnalysis : public ExitHooks, public MmapHooks, public ThreadHooks, public MallocHooks, public EnterExitFunctionHooks
{
	public:
		ProcessLevelAnalysis(bool threadSafe = true);
		virtual ~ProcessLevelAnalysis ( void );
		void init(void);
		//common
		void onAlloc(MallocHooksInfos& info, void* ret, size_t size);
		void onFreeMem(MallocHooksInfos & info, void * ptr);
		void updateMemStats( MATT::MallocHooksInfos& info, void* ret, size_t size, bool alloc );
		//exit
		virtual void onExit ( void );
		//mmap
		virtual bool mmapCallEnterExit ( void );
		virtual void onMmap ( MmapHooksInfos& info, void* res, void* start, size_t length, int prot, int flags, int fd, size_t offset );
		virtual void onMmapEnterFunction ( MmapHooksInfos& info );
		virtual void onMmapExitFunction ( MmapHooksInfos& info );
		virtual void onMunmap ( MmapHooksInfos& info, int ret, void* start, size_t length );
		virtual void onMremap ( MmapHooksInfos& info, void* ret, void* old_address, size_t old_size, size_t new_size, int flags );
		//thread
		virtual void onThreadCreate ( void );
		virtual void onThreadExit ( void );
		//malloc
		virtual bool mallocCallEnterExit ( void );
		virtual void onAlignedAlloc ( MallocHooksInfos& info, void* ret, size_t alignment, size_t size );
		virtual void onCalloc ( MallocHooksInfos& info, void* ret, size_t nmemb, size_t size );
		virtual void onMalloc ( MallocHooksInfos& info, void* ret, size_t size );
		virtual void onFree ( MallocHooksInfos& info, void* ptr );
		virtual void onMallocExitFunction ( MallocHooksInfos& info  ,void * caller,void * function);
		virtual void onMallocEnterFunction ( MallocHooksInfos& info  ,void * caller,void * function);
		virtual void onMemalign ( MallocHooksInfos& info, void* ret, size_t alignment, size_t size );
		virtual void onPosixMemalign ( MallocHooksInfos& info, int ret, void** memptr, size_t align, size_t size );
		virtual void onPreFree ( MallocHooksInfos& info, void* ptr );
		virtual void onPreRealloc ( MallocHooksInfos& info, void* ptr, size_t size );
		virtual void onPvalloc ( MallocHooksInfos& info, void* ret, size_t size );
		virtual void onRealloc ( MallocHooksInfos& info, void* ret, void* ptr, size_t size );
		virtual void onValloc ( MallocHooksInfos& info, void* ret, size_t size );
		//enter exit function
		virtual bool isEnterExitFunction ( void );
		//enter-exit handkers
		virtual void onEnterFunction ( void* caller, void* function );
		virtual void onExitFunction ( void* caller, void* function );
		//manage tls
		ThreadLevelAnalysis * getNewThreadLevelAnalysis(void);
		StackTree * getStackTree(void);
		//output
		friend void convertToJson ( htopml::JsonState& json, const ProcessLevelAnalysis& value );
	protected:
		void runInfoToJson( htopml::JsonState& json ) const;
	private:
		bool threadSafe;
		Spinlock lock;
		ThreadLevelAnalysisVector threads;
		StackTree * stackTree;
		Clock mallocClock;
		UserSegmentTracker userSegmentTracker;
		PeakTracker globalPeakTracker;
		AllocStateStats allocStateStats;
		TimeProfiler memStats;
		TimeProfiler systemStats;
		TimeProfiler opsBandwidth;
		SymbolRegistry registry;
		GlobalVariables globalVariables;
		AllocTracer tracer;
		std::string traceFilename;
		AllocSizeDistribution allocSizeDistr;
		int activeThreads;
		int maxThreads;
		size_t freeMemoryAtStart;
		size_t cachedMemoryAtStart;
};

}

#endif //MATT_PROCESS_LEVEL_ANLYSIS_HPP
