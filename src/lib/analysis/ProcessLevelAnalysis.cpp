/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 02/2015
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
#include "ProcessLevelAnalysis.hpp"
#include <hooks/EnterExitFunctionHooks.hpp>
#include <common/FormattedMessage.hpp>
#include <core/Options.hpp>
#include <common/Helpers.hpp>
#include <common/CodeTiming.hpp>
#include <portability/OS.hpp>
#include <stacks/StackTreeMap.hpp>
#include <stacks/RLockFreeTree.hpp>
#include <common/Debug.hpp>
#include <fstream>
#include <allocators/NoFreeAllocator.hpp>

/********************  MACRO  ***********************/
#define MATT_THREAD_ID 0

/*******************  FUNCTION  *********************/
namespace MATT
{

/*******************  FUNCTION  *********************/
ProcessLevelAnalysis::ProcessLevelAnalysis ( void )
{
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
	this->mallocClock.markEvent();
	onAlloc(info,ret,size);
}

/*******************  FUNCTION  *********************/
void ProcessLevelAnalysis::onCalloc ( MallocHooksInfos& info, void* ret, size_t nmemb, size_t size )
{
	this->mallocClock.markEvent();
	onAlloc(info,ret,size);
}

/*******************  FUNCTION  *********************/
void ProcessLevelAnalysis::onExit ( void )
{
	//open output file
		//TODO manage errors
		std::ofstream out;
		Options & options = getOptions();
		
		if (options.getStackMode() == MATT_STACK_TREE_ENTER_EXIT)
			dynamic_cast<RLockFreeTree*>(stackTree)->markChildData();
		
		//config
// 		if (options.outputDumpConfig)
// 		{
// 			options.dumpConfig(FormattedMessage(options.outputName).arg(OS::getExeName()).arg(Helpers::getFileId()).arg("ini").toString().c_str());
// 		}
		
		//lua
// 		if (options.outputLua)
// 		{
// 			out.open(FormattedMessage(options.outputName).arg(OS::getExeName()).arg(Helpers::getFileId()).arg("lua").toString().c_str());
// 			CODE_TIMING("outputLua",htopml::convertToLua(out,*this,options.outputIndent));
// 			out.close();
// 		}

		//json
// 		if (options.outputJson)
// 		{
			out.open(FormattedMessage(options.outputName).arg(OS::getExeName()).arg(Helpers::getFileId()).arg("json").toString().c_str());
			CODE_TIMING("outputJson",htopml::convertToJson(out,*this,options.outputIndent));
			out.close();
// 		}

		//print timings
		#ifdef MATT_ENABLE_CODE_TIMING
		CodeTiming::printAll();
		gblInternaAlloc->printState();
		#endif //MATT_ENABLE_CODE_TIMING
}

/*******************  FUNCTION  *********************/
void ProcessLevelAnalysis::onFree ( MallocHooksInfos& info, void* ptr )
{
	UserSegment segment = this->userSegmentTracker.unregister(mallocClock,ptr);
	ticks lifetime = this->mallocClock.getLastEventTime(CLOCK_TICKS) - segment.birth;
	this->stackTree->getTypedData<CallCounter>(segment.dataHandler,MATT_ANA_ID_LIFETIME).call(lifetime);
	this->stackTree->getTypedData<CallCounter>(info.handler,MATT_ANA_ID_FREE).call(segment.size);
}

/*******************  FUNCTION  *********************/
void ProcessLevelAnalysis::onAlloc ( MallocHooksInfos& info, void* ret, size_t size )
{
	this->stackTree->getTypedData<CallCounter>(info.handler,MATT_ANA_ID_ALLOC).call(size);
	this->userSegmentTracker.registerChunk(ret,size,mallocClock.getLastEventTime(CLOCK_TICKS),info.dataId,MATT_THREAD_ID,info.dataHandler);
}

/*******************  FUNCTION  *********************/
void ProcessLevelAnalysis::onMalloc ( MallocHooksInfos& info, void* ret, size_t size )
{
	this->mallocClock.markEvent();
	this->onAlloc(info,ret,size);
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
	this->mallocClock.markEvent();
	this->onAlloc(info,ret,size);
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
	this->mallocClock.markEvent();
	this->onAlloc(info,*memptr,size);
}

/*******************  FUNCTION  *********************/
void ProcessLevelAnalysis::onPreFree ( MallocHooksInfos& info, void* ptr )
{
	this->mallocClock.markEvent();
}

/*******************  FUNCTION  *********************/
void ProcessLevelAnalysis::onPreRealloc ( MallocHooksInfos& info, void* ptr, size_t size )
{
	this->mallocClock.markEvent();
}

/*******************  FUNCTION  *********************/
void ProcessLevelAnalysis::onPvalloc ( MallocHooksInfos& info, void* ret, size_t size )
{
	this->mallocClock.markEvent();
	this->onAlloc(info,ret,size);
}

/*******************  FUNCTION  *********************/
void ProcessLevelAnalysis::onRealloc ( MallocHooksInfos& info, void* ret, void* ptr, size_t size )
{
	this->mallocClock.markEvent();
	this->onAlloc(info,ret,size);
}

/*******************  FUNCTION  *********************/
void ProcessLevelAnalysis::onThreadCreate ( void )
{

}

/*******************  FUNCTION  *********************/
void ProcessLevelAnalysis::onThreadExit ( void )
{

}

/*******************  FUNCTION  *********************/
void ProcessLevelAnalysis::onValloc ( MallocHooksInfos& info, void* ret, size_t size )
{
	this->mallocClock.markEvent();
	this->onAlloc(info,ret,size);
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
		json.printField("options",getOptions());
		json.printField("stacks",*(value.stackTree));
		json.openFieldArray("threads");
			for (ThreadLevelAnalysisVector::const_iterator it = value.threads.begin() ; it != value.threads.end() ; ++it)
			{
				json.printValue(**it);
			}
		json.closeFieldArray("threads");
	json.closeStruct();
}

}
