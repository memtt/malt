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
#include "WrapperGpuCudaAlloc.hpp"
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
cudaError_enum MALT::malt_wrap_cuda_malloc(void ** devPtr, size_t size, CudaMallocPtr real_cuda_malloc, void * retaddr)
{
	//get local TLS and check init
	LazyEnv env;
	ReentranceGuard guard(env);

	//run the default function
	assert(env.getGlobalState().status > ALLOC_WRAP_INIT_SYM);
	
	ticks t = Clock::getticks();
	cudaError_enum result = real_cuda_malloc(devPtr, size);
	t = Clock::getticks() - t;

	//profile
	if (guard.needInstrument()) {
		//instru
		if (gblOptions->c.gpuMalloc) {
			MALT_WRAPPER_LOCAL_STATE_ACTION(env.getLocalProfiler().onMalloc(*devPtr,size,t,MALLOC_KIND_MALLOC, LANG_C, DOMAIN_GPU_ALLOC), retaddr);
		}
	}

	//ok
	return result;
}

/**********************************************************/
cudaError_enum MALT::malt_wrap_cuda_free(void * ptr, CudaFreePtr real_cuda_free, void * retaddr)
{
	//need to ignore them otherwise enter sometime in an infinite call loop
	//when using numpy with python
	if (ptr == NULL)
		return real_cuda_free(ptr);

	//get local TLS and check init
	LazyEnv env;
	ReentranceGuard guard(env);

	//profile
	if (guard.needInstrument() && gblOptions->c.gpuMalloc) {
		MALT_WRAPPER_LOCAL_STATE_ACTION(env.getLocalProfiler().onFree(ptr,0, LANG_C, DOMAIN_GPU_ALLOC), retaddr);
	}

	//run the default function
	return real_cuda_free(ptr);
}
