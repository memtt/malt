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

}

/*******************  FUNCTION  *********************/
bool ProcessLevelAnalysis::mallocCallEnterExit ( void )
{
}

/*******************  FUNCTION  *********************/
bool ProcessLevelAnalysis::mmapCallEnterExit ( void )
{
}

/*******************  FUNCTION  *********************/
void ProcessLevelAnalysis::onAlignedAlloc ( MallocHooksInfos& info, void* ret, size_t alignment, size_t size )
{

}

/*******************  FUNCTION  *********************/
void ProcessLevelAnalysis::onCalloc ( MallocHooksInfos& info, void* ret, size_t nmemb, size_t size )
{

}

/*******************  FUNCTION  *********************/
void ProcessLevelAnalysis::onExit ( void )
{

}

/*******************  FUNCTION  *********************/
void ProcessLevelAnalysis::onFree ( MallocHooksInfos& info, void* ptr )
{

}

/*******************  FUNCTION  *********************/
void ProcessLevelAnalysis::onMalloc ( MallocHooksInfos& info, void* ret, size_t size )
{

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

}

/*******************  FUNCTION  *********************/
void ProcessLevelAnalysis::onPreFree ( MallocHooksInfos& info, void* ptr )
{

}

/*******************  FUNCTION  *********************/
void ProcessLevelAnalysis::onPreRealloc ( MallocHooksInfos& info, void* ptr, size_t size )
{

}

/*******************  FUNCTION  *********************/
void ProcessLevelAnalysis::onPvalloc ( MallocHooksInfos& info, void* ret, size_t size )
{

}

/*******************  FUNCTION  *********************/
void ProcessLevelAnalysis::onRealloc ( MallocHooksInfos& info, void* ret, void* ptr, size_t size )
{

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

}

/*******************  FUNCTION  *********************/
bool ProcessLevelAnalysis::isEnterExitFunction ( void )
{

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
	ThreadLevelAnalysis * ret = new ThreadLevelAnalysis(this);
	ret->setInUse(true);
	return ret;
}

/*******************  FUNCTION  *********************/
StackTree* ProcessLevelAnalysis::getStackTree ( void )
{
	return stackTree;
}

}
