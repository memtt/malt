/*****************************************************
             PROJECT  : MALT
             VERSION  : 1.2.2
             DATE     : 10/2020
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
//standard
#include <cstdio>
#include <fstream>
#include <sstream>
#include <iostream>
//extension GNU
#include <execinfo.h>
//from htopml
#include <json/ConvertToJson.h>
//internal potability
#include <portability/OS.hpp>
//internal commons
#include <common/Debug.hpp>
#include <common/Helpers.hpp>
#include <common/CodeTiming.hpp>
#include <common/SimpleAllocator.hpp>
#include <common/FormattedMessage.hpp>
//locals
#include "AllocStackProfiler.hpp"
#include "LocalAllocStackProfiler.hpp"
#include <tools/NMCmdReader.hpp>
#include <wrapper/ThreadTracker.hpp>
#include "ConverterToV2Tree.hpp"
#include "common/NoFreeAllocator.hpp"
#include "stacks/StackLoopRemover.hpp"

/********************  MACROS  **********************/
#define MALT_SKIP_DEPTH 3
#define STACK_LOCATION_ID NULL

/*******************  NAMESPACE  ********************/
namespace MALT
{

/*********************  CONST ***********************/
const char * TimeTrackMemory::selfDescribeFields[5] = {
	"requestedMem",
	"physicalMem",
	"virtualMem",
	"internalMem",
	"segments",
};

/*********************  CONST ***********************/
const char * TimeTrackSysMemory::selfDescribeFields[3] = {
	"freeMemory",
	"cachedMemory",
	"swapMemory",
};

/*********************  CONST ***********************/
const char * TimeTrackAllocBandwidth::selfDescribeFields[4] = {
	"allocMem",
	"allocCount",
	"freeMem",
	"freeCount",
};

/*******************  FUNCTION  *********************/
AllocStackProfiler::AllocStackProfiler(const Options & options,StackMode mode,bool threadSafe)
	:largestStack(STACK_ORDER_DESC)
	,memoryBandwidth(1024,true)
	,sizeOverTime(64,64,false,true)
	,lifetimeOverSize(64,64,true,true)
{
	this->mode = mode;
	this->threadSafe = threadSafe;
	this->options = options;
	this->largestStackSize = 0;
	this->sharedLinearIndex = 0;
	this->stackTree = NULL;
	
	//init internal alloc
	if (gblInternaAlloc == NULL)
		gblInternaAlloc = new SimpleAllocator(true);
	
	//init tref to convert ticks in sec
	gettimeofday(&trefSec,NULL);
	trefTicks = getticks();
	
	//open trace file
	if (options.traceEnabled)
	{
		this->traceFilename = FormattedMessage(options.outputName).arg(OS::getExeName()).arg(Helpers::getFileId()).arg("trace").toString();
		tracer.open(this->traceFilename);
	}

	//get memory state at start
	OSMemUsage mem = OS::getMemoryUsage();
	this->osTotalMemory = mem.totalMemory;
	this->osFreeMemoryAtStart = mem.freeMemory;
	this->osCachedMemoryAtStart = mem.cached;
	
	//peak tracking
	this->peakId = 0;
	this->peak = 0;
	this->curReq = 0;
	
	//if request stack tree for more compressed output prepare it
	if (options.outputStackTree)
		this->stackTree = new MALTV2::RLockFreeTree(false);
}

/*******************  FUNCTION  *********************/
void AllocStackProfiler::setRealMallocAddr(MallocFuncPtr realMallocFunc)
{
	this->realMallocAddr = realMallocFunc;
}

/*******************  FUNCTION  *********************/
void AllocStackProfiler::onMalloc(void* ptr, size_t size,Stack * userStack)
{
	onAllocEvent(ptr,size,userStack);
}

/*******************  FUNCTION  *********************/
void AllocStackProfiler::onCalloc(void* ptr, size_t nmemb, size_t size,Stack * userStack)
{
	onAllocEvent(ptr,size * nmemb,userStack);
}

/*******************  FUNCTION  *********************/
void AllocStackProfiler::onFree(void* ptr,Stack * userStack)
{
	if (ptr != NULL)
		onFreeEvent(ptr,userStack);
}

/*******************  FUNCTION  *********************/
void AllocStackProfiler::registerMaqaoFunctionSymbol(int funcId, const char* funcName, const char* file, int line)
{
	symbolResolver.registerMaqaoFunctionSymbol(funcId,funcName,file,line);
}

/*******************  FUNCTION  *********************/
void AllocStackProfiler::onPrepareRealloc(void* oldPtr,Stack * userStack)
{
	//nothing to unregister, skip
}

/*******************  FUNCTION  *********************/
LocalAllocStackProfiler* AllocStackProfiler::createLocalStackProfiler(bool reentrant)
{
	LocalAllocStackProfiler* res = new LocalAllocStackProfiler(this,reentrant);
	this->registerPerThreadProfiler(res);
	return res;
}

/*******************  FUNCTION  *********************/
void AllocStackProfiler::destroyLocalStackProfiler(LocalAllocStackProfiler* localProfiler)
{
	//TODO need to track used and unused to recycle, but never really delete as we need it to
	//dump state at execution end. 
}

/*******************  FUNCTION  *********************/
size_t AllocStackProfiler::onRealloc(void* oldPtr, void* ptr, size_t newSize,Stack * userStack)
{
	size_t oldSize = 0;

	MALT_OPTIONAL_CRITICAL(lock,threadSafe)
		//to avoid to search it 2 times
		MMCallStackNode callStackNode;
		
		//free part
		if (oldPtr != NULL)
			oldSize = onFreeEvent(oldPtr,userStack,&callStackNode,false);
		
		//alloc part
		if (newSize > 0)
			onAllocEvent(ptr,newSize,userStack,&callStackNode,false);
		
		//realloc
		if (newSize > 0 && oldSize > 0 && newSize != oldSize)
		{
			//search if not provided
			if (!callStackNode.valid())
				callStackNode = getStackNode(userStack);
			
			callStackNode.infos->onReallocEvent(oldSize,newSize);
		}
		
		//register size jump
		if (options.distrReallocJump)
		{
			ReallocJump jump = {oldSize,newSize};
			ReallocJumpMap::iterator it = reallocJumpMap.find(jump);
			if (it == reallocJumpMap.end())
				reallocJumpMap[jump] = 1;
			else
				it->second++;
		}
	MALT_END_CRITICAL
	
	return oldSize;
}

/*******************  FUNCTION  *********************/
void AllocStackProfiler::onAllocEvent(void* ptr, size_t size,Stack* userStack,MMCallStackNode * callStackNode,bool doLock)
{
	//locals
	ticks t = getticks();
	MMCallStackNode localCallStackNode;
	if (callStackNode == NULL)
		callStackNode = &localCallStackNode;
	
	MALT_OPTIONAL_CRITICAL(lock,threadSafe && doLock)
		//update shared linear index
		this->sharedLinearIndex++;
		this->memOpsLevels();
		
		//peak tracking
		peakTracking(size);
	
		//update mem usage
		if (options.timeProfileEnabled)
		{
			CODE_TIMING("timeProfileAllocStart",
				curMemoryTimeline.segments++;
				curMemoryTimeline.requestedMem+=size;
				if (memoryTimeline.isNewPoint(t))
				{
					OSProcMemUsage mem = OS::getProcMemoryUsage();
					curMemoryTimeline.virtualMem = mem.virtualMemory - gblInternaAlloc->getTotalMemory();
					curMemoryTimeline.physicalMem = mem.physicalMemory - gblInternaAlloc->getTotalMemory();
				}
				sizeOverTime.push(t-trefTicks,size);
			);
		}
	
		if (options.stackProfileEnabled)
		{
			//search if not provided
			if (!callStackNode->valid())
				*callStackNode = getStackNode(userStack);
			
			//count events
			CODE_TIMING("updateInfoAlloc",callStackNode->infos->onAllocEvent(size,peakId));
		}

		//register for segment history tracking
		if (ptr != NULL)
			CODE_TIMING("segTracerAdd",segTracker.add(ptr,size,*callStackNode));
		
		//update size map
		if (options.distrAllocSize)
		{
			CODE_TIMING("sizeDistr",
				AllocSizeDistrMap::iterator it = sizeMap.find(size);
				if (it == sizeMap.end())
					sizeMap[size] = 1;
				else
					it->second++;
			);
		}
	
		//update intern mem usage
		if (options.timeProfileEnabled)
		{
			curMemoryTimeline.internalMem = gblInternaAlloc->getInuseMemory();
			memoryTimeline.push(t,curMemoryTimeline,(void*)callStackNode->stack);
		}
		
		//track alloc bandwidth
		TimeTrackAllocBandwidth allocBw;
		allocBw.allocCount = 1;
		allocBw.allocMem = size;
		memoryBandwidth.push(t,allocBw,(void*)callStackNode->stack);
	MALT_END_CRITICAL
}

/*******************  FUNCTION  *********************/
size_t AllocStackProfiler::onFreeEvent(void* ptr, MALT::Stack* userStack, MMCallStackNode* callStackNode, bool doLock)
{
	//locals
	ticks t = getticks();
	size_t size = 0;
	MMCallStackNode localCallStackNode;
	if (callStackNode == NULL)
		callStackNode = &localCallStackNode;

	MALT_OPTIONAL_CRITICAL(lock,threadSafe && doLock)
		//update shared linear index
		this->sharedLinearIndex++;
		this->memOpsLevels();

		//search segment info to link with previous history
		SegmentInfo * segInfo = NULL;
		if (options.timeProfileEnabled || options.stackProfileEnabled)
			CODE_TIMING("segTracerGet",segInfo = segTracker.get(ptr));
		
		//check unknown
		if (segInfo == NULL)
		{
			//fprintf(stderr,"Caution, get unknown free segment : %p, ingore it.\n",ptr);
			return 0;
		}
		
			
		//update mem usage
		size = segInfo->size;
		ticks lifetime = segInfo->getLifetime();
		
		//peak tracking
		peakTracking(-size);
		
		if (options.stackProfileEnabled)
		{
			//chart
			lifetimeOverSize.push(size,lifetime);
			
			//search call stack info if not provided
			if (!callStackNode->valid())
				*callStackNode = getStackNode(userStack);
			
			//count events
			CODE_TIMING("updateInfoFree",callStackNode->infos->onFreeEvent(size,peakId));
			
			//update alive (TODO, need to move this into a new function on StackNodeInfo)
			CODE_TIMING("freeLinkedMemory",segInfo->callStack.infos->onFreeLinkedMemory(size,lifetime,peakId));
		}
		
		//trace
		if (options.traceEnabled)
			tracer.traceChunk(segInfo->callStack.stack,callStackNode->stack,ptr,size,segInfo->allocTime - trefTicks,lifetime);
		
		//remove tracking info
		CODE_TIMING("segTracerRemove",segTracker.remove(ptr));
		
		//update timeline
		if (options.timeProfileEnabled)
		{
			//progr internal memory
			if (memoryTimeline.isNewPoint(t))
			{
				OSProcMemUsage mem = OS::getProcMemoryUsage();
				curMemoryTimeline.virtualMem = mem.virtualMemory - gblInternaAlloc->getTotalMemory();
				curMemoryTimeline.physicalMem = mem.physicalMemory - gblInternaAlloc->getTotalMemory();
			}
			curMemoryTimeline.segments--;
			curMemoryTimeline.internalMem = gblInternaAlloc->getInuseMemory();
			curMemoryTimeline.requestedMem -= size;
			memoryTimeline.push(t,curMemoryTimeline,(void*)callStackNode->stack);

			//system memory
			if (systemTimeline.isNewPoint(t))
			{
				OSMemUsage sysMem = OS::getMemoryUsage();
				curSystemTimeline.cachedMemory = sysMem.cached;
				curSystemTimeline.freeMemory = sysMem.freeMemory;
				curSystemTimeline.swapMemory = sysMem.swap;
				systemTimeline.push(t,curSystemTimeline,(void*)callStackNode->stack);
			}
		}
		
		//free badnwidth
		TimeTrackAllocBandwidth allocBw;
		allocBw.freeCount = 1;
		allocBw.freeMem = size;
		memoryBandwidth.push(t,allocBw,(void*)callStackNode->stack);
	MALT_END_CRITICAL
	
	return size;
}

/*******************  FUNCTION  *********************/
void AllocStackProfiler::onMmap ( void* ptr, size_t size, Stack* userStack )
{
	if (options.stackProfileEnabled)
	{
		vmaTracker.mmap(ptr,size);
		//search call stack info if not provided
		MMCallStackNode callStackNode = getStackNode(userStack);
		callStackNode.infos->onMmap(size);
	}
}

/*******************  FUNCTION  *********************/
void AllocStackProfiler::peakTracking(ssize_t delta)
{
	if (this->curReq > this->peak)
		{
			this->peakId++;
			this->peak = this->curReq;
		}
		this->curReq += delta;
}

/*******************  FUNCTION  *********************/
MMCallStackNode AllocStackProfiler::getStackNode(Stack* userStack)
{
	MMStackMap::Node * node;
	CODE_TIMING("searchInfo",node = &stackTracker.getNode(*userStack));
	MMCallStackNode res(node->first.stack,&node->second);
	return res;
}

/*******************  FUNCTION  *********************/
void AllocStackProfiler::solvePerThreadSymbols()
{
	for (LocalAllocStackProfilerList::const_iterator it = perThreadProfiler.begin() ; it != perThreadProfiler.end() ; ++it)			
		(*it)->solveSymbols(symbolResolver);
}

/*******************  FUNCTION  *********************/
void AllocStackProfiler::updatePeakInfoOfStacks(void)
{
	//fprintf(stderr,"peak = %zu , peakId = %zu\n",peak,peakId);
	for (StackSTLHashMap<CallStackInfo>::iterator it = stackTracker.begin() ; it != stackTracker.end() ; ++it)
		it->second.updatePeak(peakId);
}

/*******************  FUNCTION  *********************/
void AllocStackProfiler::loadGlobalVariables(void)
{
	//if not have libelf
	if (!ElfReader::hasLibElf())
		fprintf(stderr,"MALT: warning, malt was compiled without libelf, you will not get global variable memory usage !\n");
	
	//load /proc/map
	LinuxProcMapReader map;
	map.load();
	
	//loop on files and load vars
	for (LinuxProcMapReader::const_iterator it = map.begin() ; it != map.end() ; ++it)
	{
		//if empty file
		if (it->file.empty())
			continue;
		
		//do not look into libmalt.so
		if (it->file.find("libmalt.so") != std::string::npos)
			continue;
		
		//file not found (TODO move to portability and used better method, or manage error in ElfReader)
		FILE * fp = fopen(it->file.c_str(),"r");
		if (fp == NULL)
			continue;
		fclose(fp);
		
		//check exist
		if (globalVariables.find(it->file) == globalVariables.end())
		{
			//fprintf(stderr,"MALT: Need to load global variables from %s\n",it->file.c_str());
			
			//open elf
			ElfReader elfReader(it->file);

			//extract
			elfReader.loadGlobalVariables(globalVariables[it->file]);
			
			//search sources
			NMCmdReader reader;
			reader.load(it->file);
			reader.findSourcesAndDemangle(globalVariables[it->file]);
		}
	}
}

/*******************  FUNCTION  *********************/
void AllocStackProfiler::loopSuppress(void)
{
	//search max size
	int size = 0;
	for (StackSTLHashMap<CallStackInfo>::iterator it = this->stackTracker.begin(); it != this->stackTracker.end() ; ++it)
		if (it->first.stack->getSize() > size)
			size = it->first.stack->getSize();

	//allocate suppressor
	StackLoopRemover suppressor(size);
	
	//apply
	StackSTLHashMap<CallStackInfo>::iterator it = this->stackTracker.begin();
	Stack s(*(it->first.stack));
	int cntRemovedCalls = 0;
	int cntMerges = 0;
	int cntCalls = 0;
	int cntStacks = 0;
	while (it != this->stackTracker.end())
	{
		cntStacks++;
		cntCalls+= it->first.stack->getSize();
		//copy
		s = *(it->first.stack);
		//apply loop suppress
		suppressor.removeLoops(s);
		//check if same
		if (!(s == *(it->first.stack)))
		{
			//MALT_INFO_ARG("Suppress for : %1 -> %2").arg(*(it->first.stack)).arg(s).end();
			//update counter for stats
			cntMerges++;
			cntRemovedCalls += it->first.stack->getSize() - s.getSize();
			//search and merge
			this->stackTracker[s].merge(it->second);
			StackSTLHashMap<CallStackInfo>::iterator toRemove = it++;
			//remove old
			this->stackTracker.remove(toRemove);
		} else {
			++it;
		}
	}
	
	//debug stats
	MALT_INFO_ARG("Loops suppress  : stackes : %1 / %2 , calls : %3 / %4").arg(cntMerges).arg(cntStacks).arg(cntRemovedCalls).arg(cntCalls).end();
}

/*******************  FUNCTION  *********************/
void AllocStackProfiler::onExit(void )
{
	MALT_OPTIONAL_CRITICAL(lock,threadSafe)
		//to not instrument childs
		unsetenv("LD_PRELOAD");
		
		//solve symbols
		if (options.stackResolve)
			CODE_TIMING("solveSymbols",
				this->symbolResolver.loadProcMap();
				this->solvePerThreadSymbols();
				this->stackTracker.solveSymbols(symbolResolver);
				this->symbolResolver.solveNames()
			);
		
		//check which allocator is in use
		LinuxProcMapEntry * mallocProcMapEntry = this->symbolResolver.getMapEntry((void*)(this->realMallocAddr));
		if (mallocProcMapEntry != NULL)
			this->realMallocLib = mallocProcMapEntry->file;
		else
			this->realMallocLib = "Unknown";
		

		//update global peak info
		updatePeakInfoOfStacks();
		
		//load global variables
		loadGlobalVariables();
		
		//flush
		memoryTimeline.flush();
		systemTimeline.flush();
		memoryBandwidth.flush();
		
		//if enable loop suppression
		if (this->options.outputLoopSuppress)
			this->loopSuppress();
		
		//if need stack tree for more compressed output
		if (this->stackTree != NULL)
		{
			MALTV2::doNoFreeAllocatorInit();
			convertToV2Tree(*stackTree,this->stackTracker);
			stackTree->prepareForOutput();
		}
		
		//open output file
		//TODO manage errors
		std::ofstream out;
		
		//config
		if (options.outputDumpConfig)
		{
			options.dumpConfig(FormattedMessage(options.outputName).arg(OS::getExeName()).arg(Helpers::getFileId()).arg("ini").toString().c_str());
		}
		
		//lua
		if (options.outputLua)
		{
			out.open(FormattedMessage(options.outputName).arg(OS::getExeName()).arg(Helpers::getFileId()).arg("lua").toString().c_str());
			CODE_TIMING("outputLua",htopml::convertToLua(out,*this,options.outputIndent));
			out.close();
		}

		//json
		if (options.outputJson)
		{
			out.open(FormattedMessage(options.outputName).arg(OS::getExeName()).arg(Helpers::getFileId()).arg("json").toString().c_str());
			CODE_TIMING("outputJson",htopml::convertToJson(out,*this,options.outputIndent));
			out.close();
		}

		//valgrind out
		if (options.outputCallgrind)
		{
			if (options.outputVerbosity >= MALT_VERBOSITY_DEFAULT)
				fprintf(stderr,"MALT: Prepare valgrind output...\n");
			ValgrindOutput vout;
			
			for (StackSTLHashMap<CallStackInfo>::const_iterator itMap = stackTracker.begin() ; itMap != stackTracker.end() ; ++itMap)
				vout.pushStackInfo(*(itMap->first.stack),itMap->second,symbolResolver);
			
			//stackTracer.fillValgrindOut(vout,symbolResolver);
			CODE_TIMING("outputCallgrind",vout.writeAsCallgrind(FormattedMessage(options.outputName).arg(OS::getExeName()).arg(Helpers::getFileId()).arg("callgrind").toString(),symbolResolver));
		}

		//To know it has been done
		if (options.outputVerbosity >= MALT_VERBOSITY_DEFAULT)
			fprintf(stderr,"MALT: profile dump done...\n");

		//print timings
		#ifdef MALT_ENABLE_CODE_TIMING
		CodeTiming::printAll();
		gblInternaAlloc->printState();
		#endif //MALT_ENABLE_CODE_TIMING
	MALT_END_CRITICAL
}

/*******************  FUNCTION  *********************/
void convertToJson(htopml::JsonState& json, const AllocStackProfiler& value)
{
	json.openStruct();
	
	json.openFieldStruct("run");
		json.printField("formatVersion", MALT_JSON_FORMAT_VERSION);
		json.printField("tool","malt-" MALT_VERSION MALT_VERSION_NOTE);
		json.printField("date",OS::getDateTime());
		json.printField("runtime",getticks() - value.trefTicks);
		json.printField("allocator", value.realMallocLib);
		if (value.getOptions()->traceEnabled)
			json.printField("tracefile",value.traceFilename);
		if (value.getOptions()->infoHidden == false)
		{
			json.printField("exe",OS::getExeName());
			json.printField("command",OS::getCmdLine());
			json.printField("hostname",OS::getHostname());
		}
	json.closeFieldStruct("run");
	
	json.printField("config",value.options);

	if (value.options.stackProfileEnabled)
	{
		if (value.stackTree == NULL)
			json.printField("stacks",value.stackTracker);
		else
			json.printField("stacks",*value.stackTree);
	}
	
	if (value.options.stackResolve)
		json.printField("sites",value.symbolResolver);

	if (value.options.timeProfileEnabled)
	{
		json.openFieldStruct("timeline");
			json.printField("memoryTimeline",value.memoryTimeline);
			json.printField("systemTimeline",value.systemTimeline);
			json.printField("memoryBandwidth",value.memoryBandwidth);
		json.closeFieldStruct("timeline");
		json.openFieldStruct("scatter");
			json.printField("sizeOverTime",value.sizeOverTime);
			json.printField("lifetimeOverSize",value.lifetimeOverSize);
		json.closeFieldStruct("scatter");
	}
	
	if (value.options.maxStackEnabled)
	{
		json.openFieldArray("threads");
		for (LocalAllocStackProfilerList::const_iterator it = value.perThreadProfiler.begin() ; it != value.perThreadProfiler.end() ; ++it)			
			json.printValue(**it);
		json.closeFieldArray("threads");
// 		json.openFieldStruct("maxStack");
// 		json.printField("size",value.largestStackSize);
// 		json.printField("stack",value.largestStack);
// 		json.printField("mem",value.largestStackMem);
// 		json.printField("total",value.largestStackMem.getTotalSize());
// 		json.closeFieldStruct("maxStack");
	}
	
	json.openFieldStruct("memStats");
		if (value.options.distrAllocSize)
		{
			json.openFieldStruct("sizeMap");
			for (AllocSizeDistrMap::const_iterator it = value.sizeMap.begin() ; it != value.sizeMap.end() ; ++it)			
			{
				std::stringstream out;
				out << it->first;
				json.printField(out.str().c_str(),it->second);
			}
			json.closeFieldStruct("sizeMap");
		}
		
		if (value.options.distrReallocJump)
		{
			json.openFieldArray("reallocJump");
			for (ReallocJumpMap::const_iterator it = value.reallocJumpMap.begin() ; it != value.reallocJumpMap.end() ; ++it)			
			{
				json.printListSeparator();
				json.openStruct();
				json.printField("oldSize",it->first.oldSize);
				json.printField("newSize",it->first.newSize);
				json.printField("count",it->second);
				json.closeStruct();
			}
			json.closeFieldArray("reallocJump");
		}
		
		json.printField("globalVariables",value.globalVariables);
	json.closeFieldStruct("memStats");
	
	json.openFieldStruct("globals");
		CODE_TIMING("ticksPerSecond",json.printField("ticksPerSecond",value.ticksPerSecond()));
		json.printField("totalMemory",value.osTotalMemory);
		json.printField("freeMemoryAtStart",value.osFreeMemoryAtStart);
		json.printField("cachedMemoryAtStart",value.osCachedMemoryAtStart);
		json.printField("maxThreadCount",ThreadTracker::getMaxThreadCount());
	json.closeFieldStruct("globals");
	
	json.printField("leaks",value.segTracker);

	json.closeStruct();
	//fprintf(stderr,"peakId : %zu\n",value.peakId);
}

/*******************  FUNCTION  *********************/
void AllocStackProfiler::onLargerStackSize(const StackSizeTracker& stackSizes, const Stack& stack)
{
	//current is smaller
	if (stackSizes.getSize() < this->largestStackSize)
		return;
	
	//save
	MALT_OPTIONAL_CRITICAL(largestStackLock,threadSafe)
		this->largestStackSize = stackSizes.getSize();
		this->largestStackMem = stackSizes;
		this->largestStack = stack;
		//ATT_DEBUG("update largestStack");
	MALT_END_CRITICAL;
}

/*******************  FUNCTION  *********************/
const Options* AllocStackProfiler::getOptions(void) const
{
	return &options;
}

/*******************  FUNCTION  *********************/
void AllocStackProfiler::registerPerThreadProfiler(LocalAllocStackProfiler* profiler)
{
	//errors
	MALT_ASSERT(profiler != NULL);
	
	//insert in list
	MALT_OPTIONAL_CRITICAL(lock,threadSafe)
		this->perThreadProfiler.push_back(profiler);
	MALT_END_CRITICAL;
}

/*******************  FUNCTION  *********************/
ticks AllocStackProfiler::ticksPerSecond(void) const
{
	timeval tSec;
	ticks tTicks;
	ticks res;
	
	//read
	tTicks = getticks();
	gettimeofday(&tSec,NULL);
	
	//compute delta and store
	timeval delta;
	timersub(&tSec,&trefSec,&delta);
	
	//if too chost, sleep a little and return
	if (delta.tv_sec == 0 && delta.tv_usec < 200000)
	{
		if (options.outputVerbosity >= MALT_VERBOSITY_DEFAULT)
			fprintf(stderr,"MALT: Using usleep to get better ticks <-> seconds conversion !\n");
		usleep(200000);
		res = this->ticksPerSecond();
	} else {
		res = (double)(tTicks-trefTicks)/((double)delta.tv_sec + (double)delta.tv_usec/(double)1000000.0);
	}
	
	return res;
}

/*******************  FUNCTION  *********************/
AllocTreeStrackTracer* AllocStackProfiler::getEnterExitStackTracer(void)
{
	return &treeStackTracker;
}

/*******************  FUNCTION  *********************/
void AllocStackProfiler::memOpsLevels(void)
{
	if (sharedLinearIndex == 1000000)
		fprintf(stderr,"MALT: Already seen 1M memory operations.\n");
	if (sharedLinearIndex == 10000000)
		fprintf(stderr,"MALT: Already seen 10M memory operations, MALT certainly slows your program.\n");
	else if (sharedLinearIndex == 1000000000)
		fprintf(stderr,"MALT: Already seen 1G memory operations, MALT certainly slows your program.\n");
	#ifdef __x86_64__
		else if (sharedLinearIndex == 100000000000)
			fprintf(stderr,"MALT: Already seen 100G memory operations, MALT certainly slows your program.\n");
	#endif //__x86_64__
}

/*******************  FUNCTION  *********************/
TimeTrackMemory::TimeTrackMemory()
{
	this->internalMem = 0;
	this->physicalMem = 0;
	this->requestedMem = 0;
	this->segments = 0;
}

/*******************  FUNCTION  *********************/
void convertToJson ( htopml::JsonState& json, const TimeTrackMemory& value)
{
// 	json.openStruct();
// 	json.printField("internalMem",value.internalMem);
// 	json.printField("physicalMem",value.physicalMem);
// 	json.printField("requestedMem",value.requestedMem);
// 	json.printField("segments",value.segments);
// 	json.closeStruct();
	json.openArray();
	json.printValue(value.requestedMem);
	json.printValue(value.virtualMem);
	json.printValue(value.physicalMem);
	json.printValue(value.internalMem);
	json.printValue(value.segments);
	json.closeArray();
}

/*******************  FUNCTION  *********************/
bool TimeTrackMemory::reduce ( const TimeTrackMemory& v )
{
	bool hasUpdate = false;
	if (v.internalMem > internalMem)
	{
		internalMem = v.internalMem;
		hasUpdate = true;
	}
	if (v.physicalMem > physicalMem)
	{
		physicalMem = v.physicalMem;
		hasUpdate = true;
	}
	if (v.requestedMem > requestedMem)
	{
		requestedMem = v.requestedMem;
		hasUpdate = true;
	}
	if (v.segments > segments)
	{
		segments = v.segments;
		hasUpdate = true;
	}
	if (v.virtualMem > virtualMem)
	{
		virtualMem = v.virtualMem;
		hasUpdate = true;
	}
	return hasUpdate;
}

/*******************  FUNCTION  *********************/
TimeTrackSysMemory::TimeTrackSysMemory()
{
	this->freeMemory = 0;
	this->swapMemory = 0;
	this->cachedMemory = 0;
}

/*******************  FUNCTION  *********************/
bool TimeTrackSysMemory::reduce(const TimeTrackSysMemory& v)
{
	bool hasUpdate = false;
	if (v.freeMemory > freeMemory)
	{
		freeMemory = v.freeMemory;
		hasUpdate = true;
	}
	if (v.swapMemory > swapMemory)
	{
		swapMemory = v.swapMemory;
		hasUpdate = true;
	}
	if (v.cachedMemory > cachedMemory)
	{
		cachedMemory = v.cachedMemory;
		hasUpdate = true;
	}
	return hasUpdate;
}

/*******************  FUNCTION  *********************/
void convertToJson(htopml::JsonState& json, const TimeTrackSysMemory& value)
{
	json.openArray();
	json.printValue(value.freeMemory);
	json.printValue(value.swapMemory);
	json.printValue(value.cachedMemory);
	json.closeArray();
}

/*******************  FUNCTION  *********************/
TimeTrackAllocBandwidth::TimeTrackAllocBandwidth()
{
	this->allocCount = 0;
	this->allocMem = 0;
	this->freeCount = 0;
	this->freeMem = 0;
}

/*******************  FUNCTION  *********************/
void convertToJson(htopml::JsonState& json, const TimeTrackAllocBandwidth& value)
{
	json.openArray();
	json.printValue(value.allocCount);
	json.printValue(value.allocMem);
	json.printValue(value.freeCount);
	json.printValue(value.freeMem);
	json.closeArray();
}

/*******************  FUNCTION  *********************/
TimeTrackAllocBandwidth& TimeTrackAllocBandwidth::operator+=(const TimeTrackAllocBandwidth& value)
{
	allocCount += value.allocCount;
	allocMem += value.allocMem;
	freeCount += value.freeCount;
	freeMem += value.freeMem;
	return *this;
}

/*******************  FUNCTION  *********************/
bool TimeTrackAllocBandwidth::reduce(const TimeTrackAllocBandwidth & v)
{
	allocCount += v.allocCount;
	allocMem += v.allocMem;
	freeCount += v.freeCount;
	freeMem += v.freeMem;
	return true;
}

}
