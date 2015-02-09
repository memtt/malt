/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MATT_INIT_HPP
#define MATT_INIT_HPP

/********************  HEADERS  *********************/
#include <hooks/ExitHooks.hpp>
#include <hooks/MallocHooks.hpp>
#include <hooks/MmapHooks.hpp>
#include <hooks/ThreadHooks.hpp>
#include <hooks/EnterExitFunctionHooks.hpp>

namespace MATT
{

/*********************  CLASS  **********************/
class InitMatt : public ExitHooks, public MmapHooks, public ThreadHooks, public MallocHooks, public EnterExitFunctionHooks
{
	public:
		virtual ~InitMatt ( void );
		void init(void);
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
		virtual void onMallocExitFunction ( MallocHooksInfos& info );
		virtual void onMallocEnterFunction ( MallocHooksInfos& info );
		virtual void onMemalign ( MallocHooksInfos& info, void* ret, size_t alignment, size_t size );
		virtual void onPosixMemalign ( MallocHooksInfos& info, int ret, void** memptr, size_t align, size_t size );
		virtual void onPreFree ( MallocHooksInfos& info, void* ptr );
		virtual void onPreRealloc ( MallocHooksInfos& info, void* ptr, size_t size );
		virtual void onPvalloc ( MallocHooksInfos& info, void* ret, size_t size );
		virtual void onRealloc ( MallocHooksInfos& info, void* ret, void* ptr, size_t size );
		virtual void onValloc ( MallocHooksInfos& info, void* ret, size_t size );
		//enter exit function
		virtual bool isEnterExitFunction ( void );
		virtual void onEnterFunction ( void* caller, void* function );
		virtual void onExitFunction ( void* caller, void* function );
};

}

#endif //MATT_INIT_HPP
