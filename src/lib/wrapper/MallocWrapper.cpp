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
		gblMallocWrapperState.enterExit = MATT::gblMallocWrapperState.hooks->callEnterExit();

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
	if (MATT::gblMallocWrapperState.status == MATT::ALLOC_WRAP_NOT_INIT)
		MATT::mallocWrapperInit();

	//run the default function
	MATT_ASSERT(MATT::gblMallocWrapperState.status > MATT::ALLOC_WRAP_INIT_SYMBOL);
	MATT_ASSERT(MATT::gblMallocWrapperState.functions.malloc != NULL);
	
	ticks t = getticks();
	void * ret = MATT::gblMallocWrapperState.functions.malloc(size);
	t = getticks() - t;

	//call hoook
	MATT::MallocHooksInfos infos;
	infos.calltime = t;
	
	//enter exit
	if (!MATT::gblReenter)
	{
		MATT::gblReenter = true;
		if (MATT::gblMallocWrapperState.enterExit)
		{
			infos.retaddr = __builtin_extract_return_addr(__builtin_return_address(0));
			infos.func = (void*)__func__;
			MATT::gblMallocWrapperState.hooks->onEnterFunction(infos);
		}
		MATT::gblMallocWrapperState.hooks->onMalloc(infos,ret,size);
		if (MATT::gblMallocWrapperState.enterExit)
			MATT::gblMallocWrapperState.hooks->onExitFunction(infos);
		MATT::gblReenter = false;
	}

	//return segment to user
	return ret;
}
