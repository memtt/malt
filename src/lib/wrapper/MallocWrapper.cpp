/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
#include <cassert>
#include <cycle.h>
#include "MallocWrapper.hpp"
#include <portability/OS.hpp>
#include <common/Debug.hpp>

namespace MATT
{

/********************  MACROS  **********************/
#define MATT_MALLOC_WRAPPER_NO_ACTION do{}while(0)
#define MATT_MALLOC_WRAPPER(preaction,call,action,ret) \
	/*call hoook*/\
	MATT::MallocHooksInfos infos;\
\
	if (MATT::gblMallocWrapperState.status == MATT::ALLOC_WRAP_NOT_INIT)\
		MATT::mallocWrapperInit();\
\
	/*run the default function*/\
	MATT_ASSERT(MATT::gblMallocWrapperState.status > MATT::ALLOC_WRAP_INIT_SYMBOL);\
\
	bool reenter = MATT::gblReenter;\
	/*enter exit*/\
	if (!reenter && MATT::gblMallocWrapperState.hooks != NULL)\
	{\
		MATT::gblReenter = true;\
		if (MATT::gblMallocWrapperState.enterExit)\
		{\
			infos.retaddr = __builtin_extract_return_addr(__builtin_return_address(0));\
			infos.func = (void*)__func__;\
			MATT::gblMallocWrapperState.hooks->onMallocEnterFunction(infos);\
		}\
		preaction;\
	}\
\
	ticks t = getticks();\
	call;\
	t = getticks() - t;\
	infos.calltime = t;\
\
	/*enter exit*/\
	if (!reenter && MATT::gblMallocWrapperState.hooks != NULL)\
	{\
		action;\
		if (MATT::gblMallocWrapperState.enterExit)\
			MATT::gblMallocWrapperState.hooks->onMallocExitFunction(infos);\
		MATT::gblReenter = false;\
	}\
\
	/*return segment to user*/\
	ret

/********************* GLOBALS **********************/
MallocWrapperState gblMallocWrapperState = {ALLOC_WRAP_NOT_INIT,MATT_STATIC_MUTEX_INIT,NULL};
static __thread bool gblReenter = false;

/*******************  FUNCTION  *********************/
void mallocWrapperInit(void)
{
	//secure in case of first call in threads
	gblMallocWrapperState.lock.lock();

	if (gblMallocWrapperState.status == ALLOC_WRAP_NOT_INIT)
	{
		//update state
		gblMallocWrapperState.status = ALLOC_WRAP_INIT_SYMBOL;
		
		//search addresses
		gblMallocWrapperState.functions.malloc = OS::dlsymNext<MallocFuncPtr>("malloc");
		gblMallocWrapperState.functions.free = OS::dlsymNext<FreeFuncPtr>("free");
		gblMallocWrapperState.functions.calloc = OS::dlsymNext<CallocFuncPtr>("calloc");
		gblMallocWrapperState.functions.realloc = OS::dlsymNext<ReallocFuncPtr>("realloc");
		gblMallocWrapperState.functions.posix_memalign = OS::dlsymNext<PosixMemalignFuncPtr>("posix_memalign");
		gblMallocWrapperState.functions.aligned_alloc = OS::dlsymNext<AlignedAllocFuncPtr>("aligned_alloc");
		gblMallocWrapperState.functions.valloc = OS::dlsymNext<VallocFuncPtr>("valloc");
		gblMallocWrapperState.functions.memalign = OS::dlsymNext<MemalignFuncPtr>("memalign");
		gblMallocWrapperState.functions.pvalloc = OS::dlsymNext<PVallocFuncPtr>("pvalloc");

		//init profiler
		gblMallocWrapperState.status = ALLOC_WRAP_INIT_HOOKS;
		
		//init internal alloc
		gblMallocWrapperState.hooks = mallocHookInit();

		//check mode
		gblMallocWrapperState.enterExit = MATT::gblMallocWrapperState.hooks->mallocCallEnterExit();

		//final state
		gblMallocWrapperState.status = ALLOC_WRAP_READY;
	}

	//secure in case of first call in threads
	gblMallocWrapperState.lock.unlock();
}

}

/*******************  FUNCTION  *********************/
/**
 * Wrapper of the malloc function to capture allocations. The original symbol will be
 * search by dlsym() in AllocWrapperGlobal::init() .
 * 
 * @param size Size of the element to allocate.
 * @return Pointer to the allocated memory, NULL in case of error.
**/
void * malloc(size_t size)
{
	MATT_MALLOC_WRAPPER(
		MATT_MALLOC_WRAPPER_NO_ACTION,
		void * ret = MATT::gblMallocWrapperState.functions.malloc(size),
		MATT::gblMallocWrapperState.hooks->onMalloc(infos,ret,size),
		return ret;
	);
}

/*******************  FUNCTION  *********************/
/**
 * Wrapper of the free function to capture deallocations. The original symbol will be
 * search by dlsym() in AllocWrapperGlobal::init() .
 * 
 * @param ptr Pointer to the memory segment to released.
**/
void free(void * ptr)
{
	MATT_MALLOC_WRAPPER(
		MATT::gblMallocWrapperState.hooks->onPreFree(infos,ptr),
		MATT::gblMallocWrapperState.functions.free(ptr),
		MATT::gblMallocWrapperState.hooks->onFree(infos,ptr),
		return;
	);
}

/*******************  FUNCTION  *********************/
/**
 * Wrapper of the calloc function to capture allocations. The original symbol will be
 * search by dlsym() in AllocWrapperGlobal::init() .
 * 
 * @param nmemb Number of element to allocate.
 * @param size Size of the element to allocate.
 * @return Pointer to the allocated memory, NULL in case of error.
**/
void * calloc(size_t nmemb,size_t size)
{
	//Special trick to break infinit call look on first calloc
	//This is due to dlsym which use calloc internaly
	if (MATT::gblMallocWrapperState.status == MATT::ALLOC_WRAP_INIT_SYMBOL)
	{
		static char gblCallocIniBuffer[4096];
		MATT_ASSERT(size < sizeof(gblCallocIniBuffer));
		return gblCallocIniBuffer;
	}
	
	//standard mode
	MATT_MALLOC_WRAPPER(
		MATT_MALLOC_WRAPPER_NO_ACTION,
		void * ret = MATT::gblMallocWrapperState.functions.calloc(nmemb,size),
		MATT::gblMallocWrapperState.hooks->onCalloc(infos,ret,nmemb,size),
		return ret;
	);
}

/*******************  FUNCTION  *********************/
/**
 * Wrapper of the realloc function to capture allocations. The original symbol will be
 * search by dlsym() in AllocWrapperGlobal::init() .
 * 
 * @param ptr Address of the old segment to resize, NULL to allocate a new one.
 * @param size New requested size, 0 to free the given segment.
 * @return Pointer to the allocated memory, NULL in case of error.
**/
void * realloc(void * ptr, size_t size)
{
	MATT_MALLOC_WRAPPER(
		MATT::gblMallocWrapperState.hooks->onPreRealloc(infos,ptr,size),
		void * ret = MATT::gblMallocWrapperState.functions.realloc(ptr,size),
		MATT::gblMallocWrapperState.hooks->onRealloc(infos,ret,ptr,size),
		return ret;
	);
}

/*******************  FUNCTION  *********************/
/**
 * Wrapper of the posix_memalign function to capture allocations. The original symbol will be
 * search by dlsym() in AllocWrapperGlobal::init() .
**/
int posix_memalign(void ** memptr,size_t align, size_t size)
{
	MATT_MALLOC_WRAPPER(
		MATT_MALLOC_WRAPPER_NO_ACTION,
		int ret = MATT::gblMallocWrapperState.functions.posix_memalign(memptr,align,size),
		MATT::gblMallocWrapperState.hooks->onPosixMemalign(infos,ret,memptr,align,size),
		return ret;
	);
}

/*******************  FUNCTION  *********************/
/**
 * Wrapper of the aligned_alloc function to capture allocations. The original symbol will be
 * search by dlsym() in AllocWrapperGlobal::init() .
**/
void * aligned_alloc(size_t alignment, size_t size)
{
	MATT_MALLOC_WRAPPER(
		MATT_MALLOC_WRAPPER_NO_ACTION,
		void * ret = MATT::gblMallocWrapperState.functions.aligned_alloc(alignment, size),
		MATT::gblMallocWrapperState.hooks->onAlignedAlloc(infos,ret,alignment, size),
		return ret;
	);
}

/*******************  FUNCTION  *********************/
/**
 * Wrapper of the memalign function to capture allocations. The original symbol will be
 * search by dlsym() in AllocWrapperGlobal::init() .
**/
void *memalign(size_t alignment, size_t size)
{
	MATT_MALLOC_WRAPPER(
		MATT_MALLOC_WRAPPER_NO_ACTION,
		void * ret = MATT::gblMallocWrapperState.functions.memalign(alignment, size),
		MATT::gblMallocWrapperState.hooks->onMemalign(infos,ret,alignment, size),
		return ret;
	);
}

/*******************  FUNCTION  *********************/
/**
 * Wrapper of the valloc function to capture allocations. The original symbol will be
 * search by dlsym() in AllocWrapperGlobal::init() .
**/
void *valloc(size_t size)
{
	MATT_MALLOC_WRAPPER(
		MATT_MALLOC_WRAPPER_NO_ACTION,
		void * ret = MATT::gblMallocWrapperState.functions.valloc(size),
		MATT::gblMallocWrapperState.hooks->onValloc(infos,ret,size),
		return ret;
	);
}

/*******************  FUNCTION  *********************/
/**
 * Wrapper of the valloc function to capture allocations. The original symbol will be
 * search by dlsym() in AllocWrapperGlobal::init() .
**/
void *pvalloc(size_t size)
{
	MATT_MALLOC_WRAPPER(
		MATT_MALLOC_WRAPPER_NO_ACTION,
		void * ret = MATT::gblMallocWrapperState.functions.pvalloc(size),
		MATT::gblMallocWrapperState.hooks->onPvalloc(infos,ret,size),
		return ret;
	);
}
