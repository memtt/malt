/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 09/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/injectors/InjectCAlloc.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
#include <stdarg.h>
#include <sys/mman.h>
#include "state/GlobalState.hpp"
#include "wrappers/WrapperGpuAlloc.hpp"
#include "portability/Visibility.hpp"
#include <cuda.h>

/**********************************************************/
using namespace MALT;

/**********************************************************/
extern "C" {
	DLL_PUBLIC cudaError_enum cudaMalloc (void ** devPtr, size_t  size)
	{
		return malt_wrap_cuda_malloc(devPtr, size, gblState.gpuFuncs.cudaMalloc, MALT_RETADDR);
	}
}

/**********************************************************/
extern "C" {
	DLL_PUBLIC cudaError_enum cudaFree (void * devPtr)
	{
		return malt_wrap_cuda_free(devPtr, gblState.gpuFuncs.cudaFree, MALT_RETADDR);
	}
}

/**********************************************************/
//__asm__(".symver cudaMalloc, cudaMalloc@libcudart.so.12");
//__asm__(".symver cudaFree, cudaFree@libcudart.so.12");
