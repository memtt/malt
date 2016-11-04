/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 02/2015
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
//std c++
#include <fstream>
//matt
#include <common/Debug.hpp>
#include <core/Options.hpp>
#include <common/Helpers.hpp>
#include <portability/OS.hpp>
#include <common/CodeTiming.hpp>
#include <stacks/StackTreeMap.hpp>
#include <stacks/RLockFreeTree.hpp>
#include <allocators/NoFreeAllocator.hpp>
//current
#include "ProcessLevelAnalysis.hpp"

/********************  MACRO  ***********************/
#define MATT_THREAD_ID 0

/*******************  FUNCTION  *********************/
namespace MATT
{

/*******************  FUNCTION  *********************/
ProcessLevelAnalysis::ProcessLevelAnalysis ( bool threadSafe )
	:memStats(MATT_MEM_STATS_COUNT_ENTRIES,MATT_TIME_PROFILER_DEFAULT_SIZE,false)
	,systemStats(MATT_SYS_STATS_COUNT_ENTRIES,MATT_TIME_PROFILER_DEFAULT_SIZE,false)
	,opsBandwidth(MATT_OPS_BW_COUNT_ENTRIES,MATT_TIME_PROFILER_DEFAULT_SIZE,true)
{
	this->threadSafe = threadSafe;
	this->activeThreads = 1;
	this->maxThreads = 1;
	OSMemUsage usage = OS::getMemoryUsage();
	this->freeMemoryAtStart = usage.freeMemory;
	this->cachedMemoryAtStart = usage.cached;
	this->init();
}

/*******************  FUNCTION  *********************/
ProcessLevelAnalysis::~ProcessLevelAnalysis ( void )
{

}

/*******************  FUNCTION  *********************/
void ProcessLevelAnalysis::init ( void )
{
	Options & options = getOptions();
	void * ptr;
	switch(options.getStackMode())
	{
		case MATT_STACK_MAP_BACKTRACE:
			ptr = MATT_NO_FREE_MALLOC(sizeof(StackTreeMap));
			this->stackTree = new (ptr) StackTreeMap();
			break;
		case MATT_STACK_TREE_ENTER_EXIT:
			ptr = MATT_NO_FREE_MALLOC(sizeof(RLockFreeTree));
			this->stackTree = new (ptr) RLockFreeTree();
			break;
		case MATT_STACK_MAP_ENTER_EXIT:
			ptr = MATT_NO_FREE_MALLOC(sizeof(StackTreeMap));
			this->stackTree = new (ptr) StackTreeMap(false);
			break;
		default:
			MATT_FATAL_ARG("Invalid stck tree mode from options : %1 ! ").arg(options.getStackMode()).end();
	}
	
	//setup tree
	int id = this->stackTree->addDescriptor<CallCounter>("alloc");
	assumeArg(id == MATT_ANA_ID_ALLOC,"Do not get valid ID while registering descriptors : %1 != %2").arg(id).arg(MATT_ANA_ID_ALLOC).end();
	
	id = this->stackTree->addDescriptor<CallCounter>("free");
	assumeArg(id == MATT_ANA_ID_FREE,"Do not get valid ID while registering descriptors : %1 != %2").arg(id).arg(MATT_ANA_ID_FREE).end();
	
	id = this->stackTree->addDescriptor<CallCounter>("lifetime");
	assumeArg(id == MATT_ANA_ID_LIFETIME,"Do not get valid ID while registering descriptors : %1 != %2").arg(id).arg(MATT_ANA_ID_LIFETIME).end();
	
	id = this->stackTree->addDescriptor<StackPeakTracker>("peak");
	assumeArg(id == MATT_ANA_ID_PEAK,"Do not get valid ID while registering descriptors : %1 != %2").arg(id).arg(MATT_ANA_ID_PEAK).end();
	
	//mem stats
	this->memStats.registerEntry(MATT_MEM_STATS_REQUESTED,"requestedByMalloc");
	this->memStats.registerEntry(MATT_MEM_STATS_PHYSICAL,"processPhysical");
	this->memStats.registerEntry(MATT_MEM_STATS_VIRTUAL,"processVirtual");
	this->memStats.registerEntry(MATT_MEM_STATS_INTERNAL,"mattInternalMalloc");
	this->memStats.registerEntry(MATT_MEM_STATS_SEGMENTS,"mallocSegments");
	
	//system stats
	this->systemStats.registerEntry(MATT_SYS_STATS_FREE,"systemFreeMem");
	this->systemStats.registerEntry(MATT_SYS_STATS_CACHE,"systemCachedMem");
	this->systemStats.registerEntry(MATT_SYS_STATS_SWAP,"systemSwapedMem");
	
	//bandwidths
	this->opsBandwidth.registerEntry(MATT_OPS_BW_ALLOCATED_SIZE,"allocSize");
	this->opsBandwidth.registerEntry(MATT_OPS_BW_ALLOCATED_COUNT,"allocCount");
	this->opsBandwidth.registerEntry(MATT_OPS_BW_FREED_SIZE,"freeSize");
	this->opsBandwidth.registerEntry(MATT_OPS_BW_FREED_COUNT,"freeCount");
	
	//open trace file
	if (options.traceEnabled)
	{
		this->traceFilename = FormattedMessage(options.outputName).arg(OS::getExeName()).arg(Helpers::getFileId()).arg("trace").toString();
		this->tracer.open(this->traceFilename);
	}
}

/*******************  FUNCTION  *********************/
bool ProcessLevelAnalysis::mallocCallEnterExit ( void )
{
	return this->stackTree->isEnterExit();
}

/*******************  FUNCTION  *********************/
bool ProcessLevelAnalysis::mmapCallEnterExit ( void )
{
	return this->stackTree->isEnterExit();
}

/*******************  FUNCTION  *********************/
void ProcessLevelAnalysis::onAlignedAlloc ( MallocHooksInfos& info, void* ret, size_t alignment, size_t size )
{
	MATT_OPTIONAL_CRITICAL(lock,threadSafe)
		this->mallocClock.markEvent();
		onAlloc(info,ret,size);
	MATT_END_CRITICAL
}

/*******************  FUNCTION  *********************/
void ProcessLevelAnalysis::onCalloc ( MallocHooksInfos& info, void* ret, size_t nmemb, size_t size )
{
	MATT_OPTIONAL_CRITICAL(lock,threadSafe)
		this->mallocClock.markEvent();
		onAlloc(info,ret,size);
	MATT_END_CRITICAL
}

/*******************  FUNCTION  *********************/
void ProcessLevelAnalysis::onExit ( void )
{
	MATT_OPTIONAL_CRITICAL(lock,threadSafe)
		//open output file
		//TODO manage errors
		std::ofstream out;
		Options & options = getOptions();
		
		//close trace
		tracer.close();
		
		//solve symbols
		registry.loadProcMap();
		stackTree->registerSymbols(registry);
		registry.solveNames();
		
		//preapre for output
		if (options.outputLoopSuppress)
			stackTree->setLoopSuppress();
		stackTree->prepareForOutput();
		
		//leak summary
		userSegmentTracker.buildLeakSummary(this->leakSummary);
		
		//load global variables
		globalVariables.load();
		
		//config
		if (options.outputDumpConfig)
		{
			options.dumpConfig(FormattedMessage(options.outputName).arg(OS::getExeName()).arg(Helpers::getFileId()).arg("ini").toString().c_str());
		}
		
		//lua
// 		if (options.outputLua)
// 		{
// 			out.open(FormattedMessage(options.outputName).arg(OS::getExeName()).arg(Helpers::getFileId()).arg("lua").toString().c_str());
// 			CODE_TIMING("outputLua",htopml::convertToLua(out,*this,options.outputIndent));
// 			out.close();
// 		}

		//json
		if (options.outputJson)
		{
			out.open(FormattedMessage(options.outputName).arg(OS::getExeName()).arg(Helpers::getFileId()).arg("json").toString().c_str());
			CODE_TIMING("outputJson",htopml::convertToJson(out,*this,options.outputIndent));
			out.close();
		}

		//print timings
		#ifdef MATT_ENABLE_CODE_TIMING
		CodeTiming::printAll();
		gblInternaAlloc->printState();
		#endif //MATT_ENABLE_CODE_TIMING
	MATT_END_CRITICAL
}

/*******************  FUNCTION  *********************/
void ProcessLevelAnalysis::onFree ( MallocHooksInfos& info, void* ptr )
{
}

/*******************  FUNCTION  *********************/
void ProcessLevelAnalysis::onFreeMem ( MallocHooksInfos& info, void* ptr )
{
	//ignore NULL
	if (ptr == NULL)
		return;
	
	Options & options = getOptions();
	UserSegment segment = this->userSegmentTracker.unregister(mallocClock,ptr);
	if (segment.size != -1UL)
	{
		//track chunk lifetime and update on alloc stack info
		ticks lifetime = this->mallocClock.getLastEventTime(CLOCK_TICKS) - segment.birth;
		this->stackTree->getTypedData<CallCounter>(segment.dataHandler,MATT_ANA_ID_LIFETIME).call(lifetime);
		
		//stack stats for free
		this->stackTree->getTypedData<CallCounter>(info.handler,MATT_ANA_ID_FREE).call(segment.size);
		
		//peak tracking
		this->globalPeakTracker.update(mallocClock.getLastEventTime(CLOCK_TICKS),-segment.size);
		this->stackTree->getTypedData<StackPeakTracker>(segment.dataHandler,MATT_ANA_ID_PEAK).update(mallocClock.getLastEventTime(CLOCK_TICKS),-segment.size,this->globalPeakTracker);
		
		//update states
		this->updateMemStats(info,ptr,segment.size,false);
		
		//trace
		if (options.traceEnabled)
			this->tracer.traceChunk(segment.stackId,info.dataId,segment.size,segment.birth,lifetime);
	} else {
		MATT_WARNING_ARG("Capture non tracked segments : %1 !").arg(ptr).end();
	}
}

/*******************  FUNCTION  *********************/
void ProcessLevelAnalysis::onAlloc ( MallocHooksInfos& info, void* ret, size_t size )
{
	//update per stack allocation stats
	this->stackTree->getTypedData<CallCounter>(info.handler,MATT_ANA_ID_ALLOC).call(size);
	
	//register segment for on free info
	this->userSegmentTracker.registerChunk(ret,size,mallocClock.getLastEventTime(CLOCK_TICKS),info.dataId,MATT_THREAD_ID,info.dataHandler);
	
	//update global and per stack peak tracking
	this->globalPeakTracker.update(mallocClock.getLastEventTime(CLOCK_TICKS),size);
	this->stackTree->getTypedData<StackPeakTracker>(info.handler,MATT_ANA_ID_PEAK).update(mallocClock.getLastEventTime(CLOCK_TICKS),size,this->globalPeakTracker);
	
	//update states
	this->updateMemStats(info,ret,size,true);
	
	//size distr
	allocSizeDistr.add(size);
}

/*******************  FUNCTION  *********************/
void ProcessLevelAnalysis::updateMemStats ( MallocHooksInfos& info, void* ret, size_t size, bool alloc )
{
	//fetch some locals
	ticks time = mallocClock.get(CLOCK_TICKS);
	int dataId = info.dataId;
	//assert(dataId == stackTree->getStackId(info.dataHandler));
	
	//get virtual and physical
	OSProcMemUsage mem = OS::getProcMemoryUsage();
	size_t internalMem = gblInternaAlloc->getTotalMemory() + gblNoFreeAllocator->getTotalMemory();
	size_t virtualMem = mem.virtualMemory - internalMem;
	size_t physicalMem = mem.physicalMemory - internalMem;
	
	//updaye globals
	if (alloc)
	{
		this->allocStateStats.requestedMem += size;
		this->allocStateStats.segmentCount++;
	} else {
		this->allocStateStats.requestedMem -= size;
		this->allocStateStats.segmentCount--;
	}
	
	//update timed values
	this->memStats.push(time,dataId,MATT_MEM_STATS_REQUESTED,this->allocStateStats.requestedMem);
	this->memStats.push(time,dataId,MATT_MEM_STATS_SEGMENTS,this->allocStateStats.segmentCount);
	this->memStats.push(time,dataId,MATT_MEM_STATS_PHYSICAL,physicalMem);
	this->memStats.push(time,dataId,MATT_MEM_STATS_VIRTUAL,virtualMem);
	this->memStats.push(time,dataId,MATT_MEM_STATS_INTERNAL,internalMem);
	
	//update system info only on new points, only on free
	//alloc allocate virtual memory so might have no immediate effects on physical system memory
	if (systemStats.isNewPoint(time,MATT_SYS_STATS_FREE) && !alloc)
	{
		OSMemUsage sysMem = OS::getMemoryUsage();
		this->systemStats.push(time,dataId,MATT_SYS_STATS_FREE,sysMem.freeMemory);
		this->systemStats.push(time,dataId,MATT_SYS_STATS_CACHE,sysMem.cached);
		this->systemStats.push(time,dataId,MATT_SYS_STATS_SWAP, sysMem.swap);
	}
	
	//bandwidth
	if (alloc)
	{
		this->opsBandwidth.push(time,dataId,MATT_OPS_BW_ALLOCATED_SIZE,size);
		this->opsBandwidth.push(time,dataId,MATT_OPS_BW_ALLOCATED_COUNT,1);
	} else {
		this->opsBandwidth.push(time,dataId,MATT_OPS_BW_FREED_SIZE,size);
		this->opsBandwidth.push(time,dataId,MATT_OPS_BW_FREED_COUNT,1);
	}
}

/*******************  FUNCTION  *********************/
void ProcessLevelAnalysis::onMalloc ( MallocHooksInfos& info, void* ret, size_t size )
{
	MATT_OPTIONAL_CRITICAL(lock,threadSafe)
		this->mallocClock.markEvent();
		this->onAlloc(info,ret,size);
	MATT_END_CRITICAL
}

/*******************  FUNCTION  *********************/
void ProcessLevelAnalysis::onMallocEnterFunction ( MallocHooksInfos& info ,void * caller,void * function )
{

}

/*******************  FUNCTION  *********************/
void ProcessLevelAnalysis::onMallocExitFunction ( MallocHooksInfos& info ,void * caller,void * function )
{

}

/*******************  FUNCTION  *********************/
void ProcessLevelAnalysis::onMemalign ( MallocHooksInfos& info, void* ret, size_t alignment, size_t size )
{
	MATT_OPTIONAL_CRITICAL(lock,threadSafe)
		this->mallocClock.markEvent();
		this->onAlloc(info,ret,size);
	MATT_END_CRITICAL
}

/*******************  FUNCTION  *********************/
void ProcessLevelAnalysis::onMmap ( MmapHooksInfos& info, void* res, void* start, size_t length, int prot, int flags, int fd, size_t offset )
{

}

/*******************  FUNCTION  *********************/
void ProcessLevelAnalysis::onMmapEnterFunction ( MmapHooksInfos& info )
{

}

/*******************  FUNCTION  *********************/
void ProcessLevelAnalysis::onMmapExitFunction ( MmapHooksInfos& info )
{

}

/*******************  FUNCTION  *********************/
void ProcessLevelAnalysis::onMremap ( MmapHooksInfos& info, void* ret, void* old_address, size_t old_size, size_t new_size, int flags )
{

}

/*******************  FUNCTION  *********************/
void ProcessLevelAnalysis::onMunmap ( MmapHooksInfos& info, int ret, void* start, size_t length )
{

}

/*******************  FUNCTION  *********************/
void ProcessLevelAnalysis::onPosixMemalign ( MallocHooksInfos& info, int ret, void** memptr, size_t align, size_t size )
{
	MATT_OPTIONAL_CRITICAL(lock,threadSafe)
		this->mallocClock.markEvent();
		this->onAlloc(info,*memptr,size);
	MATT_END_CRITICAL
}

/*******************  FUNCTION  *********************/
void ProcessLevelAnalysis::onPreFree ( MallocHooksInfos& info, void* ptr )
{
	MATT_OPTIONAL_CRITICAL(lock,threadSafe)
		this->mallocClock.markEvent();
		this->onFreeMem(info,ptr);
	MATT_END_CRITICAL
}

/*******************  FUNCTION  *********************/
void ProcessLevelAnalysis::onPreRealloc ( MallocHooksInfos& info, void* ptr, size_t size )
{
	MATT_OPTIONAL_CRITICAL(lock,threadSafe)
		this->mallocClock.markEvent();
		this->onFreeMem(info,ptr);
	MATT_END_CRITICAL
}

/*******************  FUNCTION  *********************/
void ProcessLevelAnalysis::onPvalloc ( MallocHooksInfos& info, void* ret, size_t size )
{
	MATT_OPTIONAL_CRITICAL(lock,threadSafe)
		this->mallocClock.markEvent();
		this->onAlloc(info,ret,size);
	MATT_END_CRITICAL
}

/*******************  FUNCTION  *********************/
void ProcessLevelAnalysis::onRealloc ( MallocHooksInfos& info, void* ret, void* ptr, size_t size )
{
	MATT_OPTIONAL_CRITICAL(lock,threadSafe)
		this->mallocClock.markEvent();
		this->onAlloc(info,ret,size);
	MATT_END_CRITICAL
}

/*******************  FUNCTION  *********************/
void ProcessLevelAnalysis::onThreadCreate ( void )
{
	MATT_OPTIONAL_CRITICAL(lock,threadSafe)
		activeThreads++;
		if (activeThreads > maxThreads)
			maxThreads = activeThreads;
	MATT_END_CRITICAL
}

/*******************  FUNCTION  *********************/
void ProcessLevelAnalysis::onThreadExit ( void )
{
	MATT_OPTIONAL_CRITICAL(lock,threadSafe)
		activeThreads--;
		assert(activeThreads >= 1); 
	MATT_END_CRITICAL
}

/*******************  FUNCTION  *********************/
void ProcessLevelAnalysis::onValloc ( MallocHooksInfos& info, void* ret, size_t size )
{
	MATT_OPTIONAL_CRITICAL(lock,threadSafe)
		this->mallocClock.markEvent();
		this->onAlloc(info,ret,size);
	MATT_END_CRITICAL
}

/*******************  FUNCTION  *********************/
bool ProcessLevelAnalysis::isEnterExitFunction ( void )
{
	return this->stackTree->isEnterExit();
}

/*******************  FUNCTION  *********************/
void ProcessLevelAnalysis::onEnterFunction ( void* caller, void* function )
{

}

/*******************  FUNCTION  *********************/
void ProcessLevelAnalysis::onExitFunction ( void* caller, void* function )
{

}

/*******************  FUNCTION  *********************/
ThreadLevelAnalysis* ProcessLevelAnalysis::getNewThreadLevelAnalysis ( void )
{
	MATT_OPTIONAL_CRITICAL(lock,threadSafe)
		//search not in use
		for (ThreadLevelAnalysisVector::iterator it = threads.begin() ; it != threads.end() ; ++it)
		{
			if ((*it)->isInUse() == false)
			{
				(*it)->setInUse(true);
				return *it;
			}
		}
		
		//create new
		void * ptr = MATT_NO_FREE_MALLOC(sizeof(ThreadLevelAnalysis));
		ThreadLevelAnalysis * ret = new(ptr) ThreadLevelAnalysis(this);
		ret->setInUse(true);
		this->threads.push_back(ret);
		return ret;
	MATT_END_CRITICAL
}

/*******************  FUNCTION  *********************/
StackTree* ProcessLevelAnalysis::getStackTree ( void )
{
	return stackTree;
}

/*******************  FUNCTION  *********************/
void convertToJson ( htopml::JsonState& json, const ProcessLevelAnalysis& value )
{
	json.openStruct();
		value.runInfoToJson(json);
		json.printField("options",getOptions());
		json.printField("symbols",value.registry);
		json.printField("globalVariables",value.globalVariables);
		json.printField("allocSizeDistr",value.allocSizeDistr);
		json.printField("stacks",*(value.stackTree));
		json.openFieldArray("threads");
			for (ThreadLevelAnalysisVector::const_iterator it = value.threads.begin() ; it != value.threads.end() ; ++it)
			{
				json.printValue(**it);
			}
		json.closeFieldArray("threads");
		json.openFieldStruct("globals");
			json.printField("ticksPerSecond",value.mallocClock.getTicksPerSecond());
			json.printField("peak",value.globalPeakTracker);
			json.printField("totalSysMem",OS::getMemoryUsage().totalMemory);
			json.printField("freeMemoryAtStart",value.freeMemoryAtStart);
			json.printField("cachedMemoryAtStart",value.cachedMemoryAtStart);
			json.printField("maxThreads",value.maxThreads);
		json.closeFieldStruct("globals");
		json.openFieldStruct("timeline");
			json.printField("memStats",value.memStats);
			json.printField("systemStats",value.systemStats);
			json.printField("opsBandwidth",value.opsBandwidth);
		json.closeFieldStruct("timeline");
		json.printField("leaks",value.leakSummary);
	json.closeStruct();
}

/*******************  FUNCTION  *********************/
void ProcessLevelAnalysis::runInfoToJson ( htopml::JsonState& json ) const
{
	json.openFieldStruct("run");
		json.printField("formatVersion",1.0f);
			json.printField("tool","matt-0.0.0");
			json.printField("date",OS::getDateTime());
			json.printField("runtime",mallocClock.get(CLOCK_TICKS));
			if (getOptions().traceEnabled)
				json.printField("tracefile",traceFilename);
			if (getOptions().infoHidden == false)
			{
				json.printField("exe",OS::getExeName());
				json.printField("command",OS::getCmdLine());
				json.printField("hostname",OS::getHostname());
			}
	json.closeFieldStruct("run");
}

}
