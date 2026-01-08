/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 10/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/wrappers/WrapperCAlloc.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (ECR) - 2014
*    AUTHOR   : Sébastien Valat - 2014 - 2022
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
#include "state/LazyEnv.hpp"
#include "state/ReentranceGuard.hpp"
#include "WrapperGpuAlloc.hpp"
#include "common/Helpers.hpp"

/**********************************************************/
using namespace MALT;

/**********************************************************/
/** Check init status of local and global state and call enter/exit methods, then do requested action. **/
#define MALT_WRAPPER_LOCAL_STATE_ACTION(action, retAddr)  \
	do \
	{ \
		if (env.isEnterExit())\
		{\
			retAddr =__builtin_extract_return_addr(__builtin_return_address(0)); \
			env.getLocalProfiler().onEnterFunc(LangAddress(DOMAIN_C, (void*)__func__),LangAddress(DOMAIN_C, retAddr),true); \
		}\
		do{action;}while(0); \
		if (env.isEnterExit()) \
		env.getLocalProfiler().onExitFunc(LangAddress(DOMAIN_C, (void*)__func__),LangAddress(DOMAIN_C, retAddr),true); \
	} while(0)

/**********************************************************/
void MALT::malt_wrap_pgi_uacc_cuda_alloc(void ** ptr, void * info, size_t size, long async, int dindex, int error, PgiUaccCudaAllocPtr real_cuda_alloc, void * retaddr)
{
	//get local TLS and check init
	LazyEnv env;
	ReentranceGuard guard(env);

	//run the default function
	assert(env.getGlobalState().status > ALLOC_WRAP_INIT_SYM);
	
	ticks t = Clock::getticks();
	real_cuda_alloc(ptr, info, size, async, dindex, error);
	t = Clock::getticks() - t;

	//profile
	if (guard.needInstrument()) {
		//instru
		if (gblOptions->c.gpuMalloc) {
			MALT_WRAPPER_LOCAL_STATE_ACTION(env.getLocalProfiler().onMalloc(*ptr,size,t,MALLOC_KIND_MALLOC, LANG_C, DOMAIN_GPU_ALLOC), retaddr);
		}
	}
}

/**********************************************************/
void MALT::malt_wrap_pgi_uacc_cuda_free(void * ptr, long async, int dindex, int error, PgiUaccCudaFreePtr real_cuda_free, void * retaddr)
{
	//need to ignore them otherwise enter sometime in an infinite call loop
	//when using numpy with python
	if (ptr == NULL)
		return;

	//get local TLS and check init
	LazyEnv env;
	ReentranceGuard guard(env);

	//profile
	if (guard.needInstrument() && gblOptions->c.gpuMalloc) {
		MALT_WRAPPER_LOCAL_STATE_ACTION(env.getLocalProfiler().onFree(ptr,0, LANG_C, DOMAIN_GPU_ALLOC), retaddr);
	}

	//run the default function
	real_cuda_free(ptr, async, dindex, error);
}
