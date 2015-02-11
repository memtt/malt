/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 02/2015
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MATT_GLOBAL_HOOKS_NONE_HPP
#define MATT_GLOBAL_HOOKS_NONE_HPP

/********************  HEADERS  *********************/
#include <cstdlib>
#include "ExitHooks.hpp"
#include "EnterExitFunctionHooks.hpp"
#include "MallocHooks.hpp"
#include "MmapHooks.hpp"
#include "ThreadHooks.hpp"

/***************** USING NAMESPACE ******************/
namespace MATT
{

/*********************  CLASS  **********************/
class  GlobalHooksNone : public ExitHooks , public EnterExitFunctionHooks, public MallocHooks, public MmapHooks, public ThreadHooks
{
	public:
		GlobalHooksNone(){};
		virtual ~GlobalHooksNone(void) {};
		//exit
		virtual void onExit(void) {};
		//enter-exit
		virtual bool isEnterExitFunction ( void ){return false;};
		virtual void onEnterFunction ( void* caller, void* function ){};
		virtual void onExitFunction ( void* caller, void* function ){};
		//malloc
		virtual bool mallocCallEnterExit ( void ){return false;};
		virtual void onAlignedAlloc ( MallocHooksInfos& info, void* ret, size_t alignment, size_t size ){};
		virtual void onCalloc ( MallocHooksInfos& info, void* ret, size_t nmemb, size_t size ){};
		virtual void onFree ( MallocHooksInfos& info, void* ptr ){};
		virtual void onMalloc ( MallocHooksInfos& info, void* ret, size_t size ){};
		virtual void onMallocEnterFunction ( MallocHooksInfos& info, void* caller, void* function ){};
		virtual void onMallocExitFunction ( MallocHooksInfos& info, void* caller, void* function ){};
		virtual void onMemalign ( MallocHooksInfos& info, void* ret, size_t alignment, size_t size ){};
		virtual void onPosixMemalign ( MallocHooksInfos& info, int ret, void** memptr, size_t align, size_t size ){};
		virtual void onPreFree ( MallocHooksInfos& info, void* ptr ){};
		virtual void onPreRealloc ( MallocHooksInfos& info, void* ptr, size_t size ){};
		virtual void onRealloc ( MallocHooksInfos& info, void* ret, void* ptr, size_t size ){};
		virtual void onValloc ( MallocHooksInfos& info, void* ret, size_t size ){};
		virtual void onPvalloc ( MallocHooksInfos& info, void* ret, size_t size ){};
		//mmap
		virtual bool mmapCallEnterExit ( void ){return false;};
		virtual void onMmap ( MmapHooksInfos& info, void* res, void* start, size_t length, int prot, int flags, int fd, size_t offset ){};
		virtual void onMmapEnterFunction ( MmapHooksInfos& info ){};
		virtual void onMmapExitFunction ( MmapHooksInfos& info ){};
		virtual void onMremap ( MmapHooksInfos& info, void* ret, void* old_address, size_t old_size, size_t new_size, int flags ){};
		virtual void onMunmap ( MmapHooksInfos& info, int ret, void* start, size_t length ){};
		//thread
		virtual void onThreadCreate ( void ){};
		virtual void onThreadExit ( void ){};
};

};

#endif //MATT_GLOBAL_HOOKS_NONE_HPP
