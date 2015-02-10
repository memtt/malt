/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 02/2015
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MATT_MALLOC_HOOKS_HPP
#define MATT_MALLOC_HOOKS_HPP

/********************  HEADERS  *********************/
#include <cstdlib>
#include <cycle.h>
#include <stacks/StackTree.hpp>

/***************** USING NAMESPACE ******************/
namespace MATT
{
	
/********************  STRUCT  **********************/
struct MallocHooksInfos
{
	StackTreeHandler handler;
	ticks calltime;
	void * retaddr;
	void * func;
};

/*********************  CLASS  **********************/
class MallocHooks
{
	public:
		virtual ~MallocHooks(void) {};
		virtual void onMalloc(MallocHooksInfos & info,void* ret,size_t size) = 0;
		virtual void onPreFree(MallocHooksInfos & info,void * ptr) = 0;
		virtual void onFree(MallocHooksInfos & info,void * ptr) = 0;
		virtual void onCalloc(MallocHooksInfos & info,void * ret,size_t nmemb,size_t size) = 0;
		virtual void onPreRealloc(MallocHooksInfos & info,void * ptr, size_t size) = 0;
		virtual void onRealloc(MallocHooksInfos & info,void * ret,void * ptr, size_t size) = 0;
		virtual void onPosixMemalign(MallocHooksInfos & info,int ret,void ** memptr,size_t align, size_t size) = 0;
		virtual void onAlignedAlloc(MallocHooksInfos & info,void * ret,size_t alignment, size_t size) = 0;
		virtual void onMemalign(MallocHooksInfos & info,void * ret,size_t alignment, size_t size) = 0;
		virtual void onValloc(MallocHooksInfos & info,void * ret,size_t size) = 0;
		virtual void onPvalloc(MallocHooksInfos & info,void * ret,size_t size) = 0;
		virtual void onMallocEnterFunction(MallocHooksInfos & info,void * caller,void * function) = 0;
		virtual void onMallocExitFunction(MallocHooksInfos & info,void * caller,void * function) = 0;
		virtual bool mallocCallEnterExit(void) = 0;
};

/*******************  FUNCTION  *********************/
MallocHooks * mallocHookInit(void);

};

#endif //MATT_MALLOC_HOOKS_HPP