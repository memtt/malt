/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 02/2015
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MATT_THREAD_LEVEL_ANALYSIS_HPP
#define MATT_THREAD_LEVEL_ANALYSIS_HPP

/********************  HEADERS  *********************/
#include <hooks/ExitHooks.hpp>
#include <hooks/MallocHooks.hpp>
#include <hooks/MmapHooks.hpp>
#include <hooks/ThreadHooks.hpp>
#include <hooks/EnterExitFunctionHooks.hpp>
#include <core/CallCounter.hpp>
#include <stacks/StackTree.hpp>

namespace MATT
{

/*********************  TYPES  **********************/
enum MemoryFunction
{
	MEM_FUNC_MALLOC,
	MEM_FUNC_CALLOC,
	MEM_FUNC_REALLOC,
	MEM_FUNC_FREE,
	MEM_FUNC_MEMALIGN,
	MEM_FUNC_POSIX_MEMALIGN,
	MEM_FUNC_VALLOC,
	MEM_FUNC_PVALLOC,
	MEM_FUNC_ALIGNED_ALLOC,
	//Caution must be last element
	MEM_FUNC_COUNT
};

/*********************  TYPES  **********************/
class ProcessLevelAnalysis;

/*********************  CLASS  **********************/
class ThreadLevelAnalysis : public MallocHooks, public EnterExitFunctionHooks
{
	public:
		ThreadLevelAnalysis(ProcessLevelAnalysis * processLevel);
		virtual ~ThreadLevelAnalysis ( void );
		//malloc
		virtual bool mallocCallEnterExit ( void );
		virtual void onAlignedAlloc ( MallocHooksInfos& info, void* ret, size_t alignment, size_t size );
		virtual void onCalloc ( MallocHooksInfos& info, void* ret, size_t nmemb, size_t size );
		virtual void onMalloc ( MallocHooksInfos& info, void* ret, size_t size );
		virtual void onFree ( MallocHooksInfos& info, void* ptr );
		virtual void onMallocExitFunction ( MallocHooksInfos& info ,void * caller,void * function);
		virtual void onMallocEnterFunction ( MallocHooksInfos& info ,void * caller,void * function );
		virtual void onMemalign ( MallocHooksInfos& info, void* ret, size_t alignment, size_t size );
		virtual void onPosixMemalign ( MallocHooksInfos& info, int ret, void** memptr, size_t align, size_t size );
		virtual void onPreFree ( MallocHooksInfos& info, void* ptr );
		virtual void onPreRealloc ( MallocHooksInfos& info, void* ptr, size_t size );
		virtual void onPvalloc ( MallocHooksInfos& info, void* ret, size_t size );
		virtual void onRealloc ( MallocHooksInfos& info, void* ret, void* ptr, size_t size );
		virtual void onValloc ( MallocHooksInfos& info, void* ret, size_t size );
		friend void convertToJson(htopml::JsonState & json,const ThreadLevelAnalysis & value);
		virtual bool isEnterExitFunction ( void );
		virtual void onEnterFunction ( void* caller, void* function );
		virtual void onExitFunction ( void* caller, void* function );
		void setupStack( MATT::MallocHooksInfos& info, int skip );
		bool isInUse(void);
		void setInUse(bool status);
	protected:
		CallTimeSizeCounter counters[MEM_FUNC_COUNT];
		bool inUse;
		ProcessLevelAnalysis * processLevel;
		StackTreeHandler stackTreeHandler;
		StackTree * stackTree;
};

}

#endif //MATT_THREAD_LEVEL_ANALYSIS_HPP
