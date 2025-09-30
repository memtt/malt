/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 09/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/profiler/AllocStackProfiler.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (ECR) - 2014
*    AUTHOR   : Sébastien Valat - 2014 - 2020
*    AUTHOR   : Sébastien Valat (CERN) - 2015
*    AUTHOR   : Sébastien Valat (INRIA) - 2023 - 2025
*    AUTHOR   : Bastien Levasseur - 2024
***********************************************************/

/**********************************************************/
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
#include <core/ThreadTracker.hpp>
#include "ConverterToV2Tree.hpp"
#include "common/NoFreeAllocator.hpp"
#include "stacks/StackLoopRemover.hpp"
#include "wrappers/WrapperCAlloc.hpp"

/**********************************************************/
#define MALT_SKIP_DEPTH 3
#define STACK_LOCATION_ID NULL

/**********************************************************/
namespace MALT
{

/**********************************************************/
const char * TimeTrackMemory::selfDescribeFields[5] = {
	"requestedMem",
	"physicalMem",
	"virtualMem",
	"internalMem",
	"segments",
};

/**********************************************************/
const char * TimeTrackSysMemory::selfDescribeFields[3] = {
	"freeMemory",
	"cachedMemory",
	"swapMemory",
};

/**********************************************************/
const char * TimeTrackAllocBandwidth::selfDescribeFields[4] = {
	"allocMem",
	"allocCount",
	"freeMem",
	"freeCount",
};

/**********************************************************/
void maltDumpOnEvent(void);

/**********************************************************/
AllocStackProfiler::AllocStackProfiler(const Options & options,StackMode mode,bool threadSafe)
	:largestStack(STACK_ORDER_DESC)
	,memoryBandwidth(1024,true)
	,sizeOverTime(64,64,false,true)
	,lifetimeOverSize(64,64,true,true)
	,trigger(options, true)
{
	//vars
	bool doDump = false;

	this->mode = mode;
	this->threadSafe = threadSafe;
	this->options = options;
	this->largestStackSize = 0;
	this->sharedLinearIndex = 0;
	this->stackTree = NULL;
	this->nbAllocSeen = 0;
	
	//init internal alloc
	if (gblInternaAlloc == NULL)
		gblInternaAlloc = new SimpleAllocator(true);
	
	//init tref to convert ticks in sec
	gettimeofday(&trefSec,NULL);
	trefTicks = Clock::getticks();
	
	//open trace file
	if (options.traceEnabled)
	{
		this->traceFilename = FormattedMessage(options.outputName).arg(this->getFileExeScriptName()).arg(Helpers::getFileId()).arg("trace").toString();
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

/**********************************************************/
void AllocStackProfiler::setRealMallocAddr(MallocFuncPtr realMallocFunc)
{
	this->realMallocAddr = realMallocFunc;
}

/**********************************************************/
void AllocStackProfiler::onMalloc(void* ptr, size_t size,Stack * userStack, AllocDomain domain)
{
	onAllocEvent(ptr,size,userStack, nullptr, true, domain);
}

/**********************************************************/
void AllocStackProfiler::onCalloc(void* ptr, size_t nmemb, size_t size,Stack * userStack, AllocDomain domain)
{
	onAllocEvent(ptr,size * nmemb,userStack, nullptr, true, domain);
}

/**********************************************************/
void AllocStackProfiler::onFree(void* ptr,Stack * userStack, AllocDomain domain)
{
	if (ptr != NULL)
		onFreeEvent(ptr,userStack, nullptr, true, domain);
}

/**********************************************************/
void AllocStackProfiler::registerMaqaoFunctionSymbol(int funcId, const char* funcName, const char* file, int line)
{
	symbolResolver.registerMaqaoFunctionSymbol(funcId,funcName,file,line);
}

/**********************************************************/
void AllocStackProfiler::onPrepareRealloc(void* oldPtr,Stack * userStack)
{
	//nothing to unregister, skip
}

/**********************************************************/
LocalAllocStackProfiler* AllocStackProfiler::createLocalStackProfiler()
{
	void * mem = MALT_MALLOC(sizeof(LocalAllocStackProfiler));
	LocalAllocStackProfiler* res = new(mem) LocalAllocStackProfiler(this);
	this->registerPerThreadProfiler(res);
	return res;
}

/**********************************************************/
void AllocStackProfiler::destroyLocalStackProfiler(LocalAllocStackProfiler* localProfiler)
{
	//TODO need to track used and unused to recycle, but never really delete as we need it to
	//dump state at execution end. 
}

/**********************************************************/
size_t AllocStackProfiler::onRealloc(void* oldPtr, void* ptr, size_t newSize,Stack * userStack, AllocDomain domain)
{
	size_t oldSize = 0;

	MALT_OPTIONAL_CRITICAL(lock,threadSafe)
		//to avoid to search it 2 times
		MMCallStackNode callStackNode;
		
		//free part
		if (oldPtr != NULL)
			oldSize = onFreeEvent(oldPtr,userStack,&callStackNode,false, domain);
		
		//alloc part
		if (newSize > 0)
			onAllocEvent(ptr,newSize,userStack,&callStackNode,false, domain);
		
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

/**********************************************************/
void AllocStackProfiler::onAllocEvent(void* ptr, size_t size,Stack* userStack,MMCallStackNode * callStackNode,bool doLock, AllocDomain domain)
{
	//locals
	bool doDump = false;
	ticks t = Clock::getticks();
	MMCallStackNode localCallStackNode;
	if (callStackNode == NULL)
		callStackNode = &localCallStackNode;
	
	MALT_OPTIONAL_CRITICAL(lock,threadSafe && doLock)
		//trigger free if python make reuse internally some blocks without passing by the garbare collector
		bool isPython = (domain == DOMAIN_PYTHON_MEM || domain == DOMAIN_PYTHON_OBJ || domain == DOMAIN_PYTHON_RAW);
		if (isPython) {
			SegmentInfo * segInfo = segTracker.get(ptr);
			if (segInfo != nullptr) {
				this->onFreeEvent(ptr, userStack, callStackNode, false, domain);
			}
		}

		//update shared linear index
		this->sharedLinearIndex++;
		this->memOpsLevels();

		//trigger
		this->nbAllocSeen++;
		this->trigger.onAllocOp(this->nbAllocSeen);
		
		//peak tracking
		peakTracking(size);

		//count
		this->domains.countAlloc(domain, size);
	
		//update mem usage
		if (options.timeProfileEnabled)
		{
			CODE_TIMING("timeProfileAllocStart",
				curMemoryTimeline.segments++;
				curMemoryTimeline.requestedMem+=size;
				doDump |= trigger.onRequestUpdate(curMemoryTimeline.requestedMem);
				if (memoryTimeline.isNewPoint(t) || size > 1024*1024)
				{
					OSProcMemUsage mem = OS::getProcMemoryUsage();
					curMemoryTimeline.virtualMem = mem.virtualMemory - gblInternaAlloc->getTotalMemory() - maltJeMallocMem;
					size_t allocMem = gblInternaAlloc->getTotalMemory() + maltJeMallocMem;
					if (mem.physicalMemory < allocMem) {
						curMemoryTimeline.physicalMem = 0;
					} else {
						curMemoryTimeline.physicalMem = mem.physicalMemory - allocMem;
					}
					doDump |= trigger.onProcMemUpdate(mem);
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
			if (domain == DOMAIN_MMAP)
				CODE_TIMING("updateInfoAlloc",callStackNode->infos->onMmap(size,peakId));
			else
				CODE_TIMING("updateInfoAlloc",callStackNode->infos->onAllocEvent(size,peakId));
		}

		//register for segment history tracking
		if (ptr != NULL) {
			if (domain == DOMAIN_MMAP) {
				CODE_TIMING("segTracerAdd",mmapSegTracker.add(ptr,size,*callStackNode));
			} else {
				CODE_TIMING("segTracerAdd",segTracker.add(ptr,size,*callStackNode));
			}
		}
		
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
			curMemoryTimeline.internalMem = gblInternaAlloc->getInuseMemory() + this->maltJeMallocMem;
			memoryTimeline.push(t,curMemoryTimeline,(void*)callStackNode->stack);
		}
		
		//track alloc bandwidth
		TimeTrackAllocBandwidth allocBw;
		allocBw.allocCount = 1;
		allocBw.allocMem = size;
		memoryBandwidth.push(t,allocBw,(void*)callStackNode->stack);
	MALT_END_CRITICAL

	//trigger dump
	if (doDump)
		maltDumpOnEvent();
}

/**********************************************************/
void AllocStackProfiler::onUpdateMem(const OSProcMemUsage & procMem, const OSMemUsage & sysMem)
{
	MALT_OPTIONAL_CRITICAL(lock,threadSafe)
		ticks t = Clock::getticks();
		if (systemTimeline.isNewPoint(t))
		{
			curSystemTimeline.cachedMemory = sysMem.cached;
			curSystemTimeline.freeMemory = sysMem.freeMemory;
			curSystemTimeline.swapMemory = sysMem.swap;
		}
		if (memoryTimeline.isNewPoint(t))
		{
			curMemoryTimeline.virtualMem = procMem.virtualMemory - gblInternaAlloc->getTotalMemory() - maltJeMallocMem;
			curMemoryTimeline.physicalMem = procMem.physicalMemory - gblInternaAlloc->getTotalMemory() - maltJeMallocMem;
			size_t allocMem = gblInternaAlloc->getTotalMemory() + maltJeMallocMem;
			if (procMem.physicalMemory < allocMem) {
				curMemoryTimeline.physicalMem = 0;
			} else {
				curMemoryTimeline.physicalMem = procMem.physicalMemory - allocMem;
			}
		}
		//update intern mem usage
		if (options.timeProfileEnabled)
		{
			curMemoryTimeline.internalMem = gblInternaAlloc->getInuseMemory() + this->maltJeMallocMem;
			memoryTimeline.push(t,curMemoryTimeline,nullptr);
			systemTimeline.push(t,curSystemTimeline,nullptr);
		}
	MALT_END_CRITICAL
}

/**********************************************************/
size_t AllocStackProfiler::onFreeEvent(void* ptr, MALT::Stack* userStack, MMCallStackNode* callStackNode, bool doLock, AllocDomain domain, bool subMunmap)
{
	//locals
	ticks t = Clock::getticks();
	size_t size = 0;
	bool doDump = false;
	MMCallStackNode localCallStackNode;
	if (callStackNode == NULL)
		callStackNode = &localCallStackNode;

	MALT_OPTIONAL_CRITICAL(lock,threadSafe && doLock)
		//update shared linear index
		this->sharedLinearIndex++;
		this->memOpsLevels();

		//search segment info to link with previous history
		SegmentInfo * segInfo = NULL;
		if (options.timeProfileEnabled || options.stackProfileEnabled) {
			if (domain == DOMAIN_MMAP) {
				CODE_TIMING("segTracerGet",segInfo = mmapSegTracker.get(ptr));
			} else {
				CODE_TIMING("segTracerGet",segInfo = segTracker.get(ptr));
			}
		}
		
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
			if (domain == DOMAIN_MMAP) {
				CODE_TIMING("updateInfoFree",callStackNode->infos->onMunmap(size,peakId, subMunmap));
			} else {
				CODE_TIMING("updateInfoFree",callStackNode->infos->onFreeEvent(size,peakId));
			}
			
			//update alive (TODO, need to move this into a new function on StackNodeInfo)
			CODE_TIMING("freeLinkedMemory",segInfo->callStack.infos->onFreeLinkedMemory(size,lifetime,peakId));
		}
		
		//trace
		if (options.traceEnabled)
			tracer.traceChunk(segInfo->callStack.stack,callStackNode->stack,ptr,size,segInfo->allocTime - trefTicks,lifetime);
		
		//remove tracking info
		if (domain == DOMAIN_MMAP) {
			CODE_TIMING("segTracerRemove",mmapSegTracker.remove(ptr));
		} else {
			CODE_TIMING("segTracerRemove",segTracker.remove(ptr));
		}
		
		//update timeline
		if (options.timeProfileEnabled)
		{
			//progr internal memory
			if (memoryTimeline.isNewPoint(t) || size > 1024UL*1024UL)
			{
				OSProcMemUsage mem = OS::getProcMemoryUsage();
				curMemoryTimeline.virtualMem = mem.virtualMemory - gblInternaAlloc->getTotalMemory() - maltJeMallocMem;
				size_t allocMem = gblInternaAlloc->getTotalMemory() + maltJeMallocMem;
				if (mem.physicalMemory < allocMem) {
					curMemoryTimeline.physicalMem = 0;
				} else {
					curMemoryTimeline.physicalMem = mem.physicalMemory - allocMem;
				}
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

				//trigger dump
				if (this->trigger.onSysUpdate(sysMem))
					doDump = true;
			}
		}
		
		//free badnwidth
		TimeTrackAllocBandwidth allocBw;
		allocBw.freeCount = 1;
		allocBw.freeMem = size;
		memoryBandwidth.push(t,allocBw,(void*)callStackNode->stack);
	MALT_END_CRITICAL
	
	//trigger dump
	if (doDump)
		maltDumpOnEvent();

	return size;
}

/**********************************************************/
void AllocStackProfiler::onMmap ( void* ptr, size_t size, Stack* userStack, MMCallStackNode* callStackNode )
{
	ssize_t delta = vmaTracker.mmap(ptr,size);
	this->onAllocEvent(ptr,delta,userStack, callStackNode, true, DOMAIN_MMAP);
}

/**********************************************************/
void AllocStackProfiler::applyVmaPatches(Stack* userStack, MMCallStackNode* callStackNode, VmaSegmentPatches & vmaPatches)
{
	//search original stack infos
	MALT_OPTIONAL_CRITICAL(lock,threadSafe)
		std::map<size_t, const SegmentInfo *> ptrTrack;
		for (auto & patch : vmaPatches) {
			if (patch.oldAddr != 0) {
				const SegmentInfo * segInfos = this->mmapSegTracker.get((void*)patch.oldAddr);
				if (segInfos == nullptr) {
					const auto it = ptrTrack.find(patch.oldAddr);
					if (it != ptrTrack.end())
						segInfos = it->second;
				}
				if (segInfos != nullptr) {
					patch.stack = segInfos->callStack.stack;
					patch.infos = segInfos->callStack.infos;
					patch.allocTime = segInfos->allocTime;
					ptrTrack[patch.newAddr] = segInfos;
				}
			}
		}

		//apply patches
		bool subMunmap = false;
		for (const auto & patch : vmaPatches) {
			if (patch.infos == nullptr) {
				fprintf(stderr,"MALT: Warning: ignore some unmapped segment due to missing stack infos !\n");
			} if (patch.oldSize == 0) {
				//insert new one
				//fprintf(stderr, "ADD %zx - %zu\n", patch.newAddr, patch.newSize);
				SegmentInfo * infos = this->mmapSegTracker.add((void*)patch.newAddr, patch.newSize, MMCallStackNode(patch.stack,patch.infos));
				infos->allocTime = patch.allocTime;
			} else if (patch.newSize == 0) {
				//free it fully
				//fprintf(stderr, "FREE %zx - %zu\n", patch.oldAddr, patch.oldSize);
				this->onFreeEvent((void*)patch.oldAddr,userStack, callStackNode, false, DOMAIN_MMAP, subMunmap);
				subMunmap = true;
			} else if (patch.oldAddr == patch.newAddr) {
				//change size of old
				SegmentInfo * segInfos = this->mmapSegTracker.get((void*)patch.oldAddr);
				segInfos->size = patch.newSize;
				//fprintf(stderr, "RSIZE %zx - %zu => %zu\n", patch.oldAddr, patch.oldSize, patch.newSize);
			} else {
				std::cerr << "MALT: This should never reach this portion of code !" << std::endl;
				std::cerr << patch << std::endl;
				abort();
			}
		}
	MALT_END_CRITICAL
}

/**********************************************************/
void AllocStackProfiler::onMunmap ( void* ptr, size_t size, Stack* userStack, MMCallStackNode* callStackNode )
{
	VmaSegmentPatches vmaPatches;
	ssize_t delta = vmaTracker.munmap(ptr,size,&vmaPatches);
	this->applyVmaPatches(userStack, callStackNode, vmaPatches);
}

/**********************************************************/
void AllocStackProfiler::onMremap(void * ptr,size_t size,void * new_ptr, size_t new_size, Stack * userStack)
{
	MMCallStackNode callStackNode;
	VmaSegmentPatches vmaPatches;
	vmaTracker.munmap(ptr,size,&vmaPatches);
	vmaTracker.munmap(new_ptr,new_size,&vmaPatches);
	this->applyVmaPatches(userStack, &callStackNode, vmaPatches);
	this->onMmap(new_ptr, new_size, userStack, &callStackNode);

	//register size jump
	if (options.distrReallocJump)
	{
		ReallocJump jump = {size,new_size};
		ReallocJumpMap::iterator it = reallocJumpMap.find(jump);
		if (it == reallocJumpMap.end())
			reallocJumpMap[jump] = 1;
		else
			it->second++;
	}
}

/**********************************************************/
void AllocStackProfiler::peakTracking(ssize_t delta)
{
	if (this->curReq > this->peak)
		{
			this->peakId++;
			this->peak = this->curReq;
		}
		this->curReq += delta;
}

/**********************************************************/
MMCallStackNode AllocStackProfiler::getStackNode(Stack* userStack)
{
	MMStackMap::Node * node;
	MMCallStackNode res;
	//CODE_TIMING("stackReducer",this->reducer.reduce(*userStack));
	CODE_TIMING("searchInfo", {
		const MMCallStackNode * cacheResult = nullptr;
		#ifdef MALT_ENABLE_CACHING
			cacheResult = this->stackTrackerCache.get(*userStack);
		#endif //MALT_ENABLE_CACHING
		if (cacheResult != nullptr) {
			res = *cacheResult;
		} else {
			node = &stackTracker.getNode(*userStack);
			MMCallStackNode tmp(node->first.stack,&node->second);
			res = tmp;
			#ifdef MALT_ENABLE_CACHING
				this->stackTrackerCache.set(node->first.stack, res);
			#endif //MALT_ENABLE_CACHING
		}
	});
	return res;
}

/**********************************************************/
void AllocStackProfiler::solvePerThreadSymbols()
{
	for (LocalAllocStackProfilerList::const_iterator it = perThreadProfiler.begin() ; it != perThreadProfiler.end() ; ++it)			
		(*it)->solveSymbols(symbolResolver);
}

/**********************************************************/
void AllocStackProfiler::updatePeakInfoOfStacks(void)
{
	//fprintf(stderr,"peak = %zu , peakId = %zu\n",peak,peakId);
	for (StackSTLHashMap<CallStackInfo>::iterator it = stackTracker.begin() ; it != stackTracker.end() ; ++it)
		it->second.updatePeak(peakId);
}

/**********************************************************/
void AllocStackProfiler::loadGlobalVariables(void)
{
	//if not have libelf
	if (!ElfReader::hasLibElf())
		fprintf(stderr,"MALT: warning, malt was compiled without libelf, you will not get global variable memory usage !\n");
	
	//load /proc/map
	LinuxProcMapReader map;
	map.load();

	//to get physical addresses
	ProcPageMapReader pageMap;

	//get max size
	size_t nmMaxFileSize = Helpers::valueFromKiloMegaGiga(gblOptions->toolsNmMaxSize);
	
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

			//extarct physical size
			for (auto & itFile : globalVariables)
				for (auto & itVar:  itFile.second)
					itVar.usedSize = elfReader.getPhysSize(map, pageMap, itVar);
			
			//file size
			size_t fsize = OS::getFileSize(it->file);

			//search sources
			if (gblOptions->toolsNm) {
				if (nmMaxFileSize == 0 || fsize <= nmMaxFileSize) {
					NMCmdReader reader;
					CODE_TIMING("nm",reader.load(it->file));
					reader.findSourcesAndDemangle(globalVariables[it->file]);
				} else if (fsize > nmMaxFileSize) {
					fprintf(stderr, "MALT: Skipping global var location analysis for '%s', file is too large (tools:nmMaxSize=%s)\n", it->file.c_str(), gblOptions->toolsNmMaxSize.c_str());
				}
			}
		}
	}
}

/**********************************************************/
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
	//MALT_INFO_ARG("Loops suppress  : stacks : %1 / %2 , calls : %3 / %4").arg(cntMerges).arg(cntStacks).arg(cntRemovedCalls).arg(cntCalls).end();
}

/**********************************************************/
bool AllocStackProfiler::isImportStack(const Stack & stack) const
{
	const auto & importAddresses = this->pythonSymbolTracker.getImportAddresses();
	for (size_t i = 0 ; i < stack.getSize() ; i++) {
		LangAddress addr = stack[i];
		if (importAddresses.find(addr) != importAddresses.end()) {
			return true;
		}
	}
	return false;
}

/**********************************************************/
void AllocStackProfiler::pythonImportSuppress(void)
{
	//apply
	StackSTLHashMap<CallStackInfo>::iterator it = this->stackTracker.begin();
	LangAddress replStack[] = {LangAddress(DOMAIN_PYTHON, MALT_PYTHON_IMPORT_ID)};
	Stack replacement(replStack, 1, STACK_ORDER_ASC);
	while (it != this->stackTracker.end())
	{
		if (this->isImportStack(*it->first.stack))
		{
			//search and merge
			this->stackTracker[replacement].merge(it->second);
			StackSTLHashMap<CallStackInfo>::iterator toRemove = it++;
			//remove old
			this->stackTracker.remove(toRemove);
		} else {
			++it;
		}
	}
}

/**********************************************************/
void AllocStackProfiler::onExit(void )
{
	MALT_OPTIONAL_CRITICAL(lock,threadSafe)
		//to not instrument childs
		unsetenv("LD_PRELOAD");

		//stop tracking threads
		bool oldTrackingValue = MALT::gblThreadTrackerData.trackingIsEnabled;
		MALT::gblThreadTrackerData.trackingIsEnabled = false;

		//register symbols
		this->pythonSymbolTracker.registerSymbolResolution(this->symbolResolver);

		//solve symbols
		CODE_TIMING("solveSymbols",
			this->symbolResolver.loadProcMap();
			this->solvePerThreadSymbols();
			this->stackTracker.solveSymbols(symbolResolver);
			if (options.stackResolve) {
				this->skipThreadRegister = true;
				this->symbolResolver.solveNames();
				this->skipThreadRegister = false;
			} else {
				this->symbolResolver.solveAslrOffsets();
			}
		);
		
		//check which allocator is in use
		LangAddress addr(DOMAIN_C, (void*) this->realMallocAddr);
		LinuxProcMapEntry * mallocProcMapEntry = this->symbolResolver.getMapEntry(addr);
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
		
		//sumpress
		if (gblOptions->pythonHideImports) {
			CODE_TIMING("importSuppress", this->pythonImportSuppress());
		}

		//if enable loop suppression
		if (this->options.outputLoopSuppress) {
			CODE_TIMING("loopSuppress", this->loopSuppress());
		}
		
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
			options.dumpConfig(FormattedMessage(options.outputName).arg(this->getFileExeScriptName()).arg(Helpers::getFileId()).arg("ini").toString().c_str());
		}
		
		//lua
		if (options.outputLua)
		{
			out.open(FormattedMessage(options.outputName).arg(this->getFileExeScriptName()).arg(Helpers::getFileId()).arg("lua").toString().c_str());
			CODE_TIMING("outputLua",htopml::convertToLua(out,*this,options.outputIndent));
			out.close();
		}

		//json
		if (options.outputJson)
		{
			out.open(FormattedMessage(options.outputName).arg(this->getFileExeScriptName()).arg(Helpers::getFileId()).arg("json").toString().c_str());
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
			CODE_TIMING("outputCallgrind",vout.writeAsCallgrind(FormattedMessage(options.outputName).arg(this->getFileExeScriptName()).arg(Helpers::getFileId()).arg("callgrind").toString(),symbolResolver));
		}

		//To know it has been done
		if (options.outputVerbosity >= MALT_VERBOSITY_DEFAULT)
			fprintf(stderr,"MALT: profile dump done : %s ...\n", FormattedMessage(options.outputName).arg(this->getFileExeScriptName()).arg(Helpers::getFileId()).arg("json").toString().c_str());

		//print timings
		#ifdef MALT_ENABLE_CODE_TIMING
		CodeTiming::printAll();
		gblInternaAlloc->printState();
		for (const auto & it : this->perThreadProfiler)
			it->printStats();
		this->segTracker.printStats();
		this->mmapSegTracker.printStats();
		this->pythonSymbolTracker.printStats();
		this->stackTrackerCache.printStats("stackTrackerCache");
		#endif //MALT_ENABLE_CODE_TIMING

		//stop tracking threads
		MALT::gblThreadTrackerData.trackingIsEnabled = oldTrackingValue;
	MALT_END_CRITICAL
}

/**********************************************************/
void convertToJson(htopml::JsonState& json, const AllocStackProfiler& value)
{
	json.openStruct();
	
	json.openFieldStruct("run");
		json.printField("formatVersion", MALT_JSON_FORMAT_VERSION);
		json.printField("tool","malt-" MALT_VERSION MALT_VERSION_NOTE);
		json.printField("date",OS::getDateTime());
		json.printField("runtime",Clock::getticks() - value.trefTicks);
		json.printField("allocator", value.realMallocLib);
		json.printField("allocatorWrappers", maltGetWrappedSymbols());
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
	
	//merge
	SegmentTracker mergedLeaks;
	mergedLeaks.merge(value.segTracker);
	mergedLeaks.merge(value.mmapSegTracker);

	json.printField("leaks",mergedLeaks);

	json.printField("domains", value.domains);

	json.closeStruct();
	//fprintf(stderr,"peakId : %zu\n",value.peakId);
}

/**********************************************************/
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

/**********************************************************/
const Options* AllocStackProfiler::getOptions(void) const
{
	return &options;
}

/**********************************************************/
void AllocStackProfiler::registerPerThreadProfiler(LocalAllocStackProfiler* profiler)
{
	//errors
	MALT_ASSERT(profiler != NULL);

	//skip
	if (skipThreadRegister)
		return;
	
	//insert in list
	MALT_OPTIONAL_CRITICAL(lock,threadSafe)
		this->perThreadProfiler.push_back(profiler);
	MALT_END_CRITICAL;
}

/**********************************************************/
ticks AllocStackProfiler::ticksPerSecond(void) const
{
	timeval tSec;
	ticks tTicks;
	ticks res;
	
	//read
	tTicks = Clock::getticks();
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

/**********************************************************/
AllocTreeStrackTracer* AllocStackProfiler::getEnterExitStackTracer(void)
{
	return &treeStackTracker;
}

/**********************************************************/
void AllocStackProfiler::memOpsLevels(void)
{
	if (sharedLinearIndex == 1000000)
		fprintf(stderr,"MALT: Already seen 1M memory operations.\n");
	else if (sharedLinearIndex == 10000000)
		fprintf(stderr,"MALT: Already seen 10M memory operations, MALT certainly slows your program.\n");
	else if (sharedLinearIndex == 100000000)
		fprintf(stderr,"MALT: Already seen 100M memory operations, MALT certainly slows your program.\n");
	else if (sharedLinearIndex == 500000000)
		fprintf(stderr,"MALT: Already seen 500M memory operations, MALT certainly slows your program.\n");
	else if (sharedLinearIndex == 1000000000)
		fprintf(stderr,"MALT: Already seen 1G memory operations, MALT certainly slows your program.\n");
	#ifdef __x86_64__
		else if (sharedLinearIndex == 100000000000)
			fprintf(stderr,"MALT: Already seen 100G memory operations, MALT certainly slows your program.\n");
	#endif //__x86_64__
}

/**********************************************************/
TimeTrackMemory::TimeTrackMemory()
{
	this->internalMem = 0;
	this->physicalMem = 0;
	this->requestedMem = 0;
	this->segments = 0;
}

/**********************************************************/
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
	json.printValue(value.physicalMem);
	json.printValue(value.virtualMem);
	json.printValue(value.internalMem);
	json.printValue(value.segments);
	json.closeArray();
}

/**********************************************************/
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

/**********************************************************/
TimeTrackSysMemory::TimeTrackSysMemory()
{
	this->freeMemory = 0;
	this->swapMemory = 0;
	this->cachedMemory = 0;
}

/**********************************************************/
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

/**********************************************************/
void convertToJson(htopml::JsonState& json, const TimeTrackSysMemory& value)
{
	json.openArray();
	json.printValue(value.freeMemory);
	json.printValue(value.swapMemory);
	json.printValue(value.cachedMemory);
	json.closeArray();
}

/**********************************************************/
TimeTrackAllocBandwidth::TimeTrackAllocBandwidth()
{
	this->allocCount = 0;
	this->allocMem = 0;
	this->freeCount = 0;
	this->freeMem = 0;
}

/**********************************************************/
void convertToJson(htopml::JsonState& json, const TimeTrackAllocBandwidth& value)
{
	json.openArray();
	json.printValue(value.allocCount);
	json.printValue(value.allocMem);
	json.printValue(value.freeCount);
	json.printValue(value.freeMem);
	json.closeArray();
}

/**********************************************************/
TimeTrackAllocBandwidth& TimeTrackAllocBandwidth::operator+=(const TimeTrackAllocBandwidth& value)
{
	allocCount += value.allocCount;
	allocMem += value.allocMem;
	freeCount += value.freeCount;
	freeMem += value.freeMem;
	return *this;
}

/**********************************************************/
bool TimeTrackAllocBandwidth::reduce(const TimeTrackAllocBandwidth & v)
{
	allocCount += v.allocCount;
	allocMem += v.allocMem;
	freeCount += v.freeCount;
	freeMem += v.freeMem;
	return true;
}

/**********************************************************/
PythonSymbolTracker & AllocStackProfiler::getPythonSymbolTracker(void)
{
	return this->pythonSymbolTracker;
}

/**********************************************************/
bool AllocStackProfiler::isEnterExit(void)
{
	return mode == STACK_MODE_ENTER_EXIT_FUNC;
}

/**********************************************************/
void AllocStackProfiler::registerMaltJeMallocMem(ssize_t value)
{
	this->maltJeMallocMem += value;
}

/**********************************************************/
std::string AllocStackProfiler::getFileExeScriptName(void) const
{
	if (this->pythonSymbolTracker.getScriptName().empty()) {
		return OS::getExeName();
	} else {
		std::string tmp = this->pythonSymbolTracker.getScriptName();
		for (auto & ch : tmp)
			if (ch == '.')
				ch = '-';
		return tmp;
	}
}

}
