/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 08/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/wrappers/WrapperCAlloc.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2022
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

#ifndef MALT_GPU_CUDA_ALLOC_WRAPPER_EXTEND_HPP
#define MALT_GPU_CUDA_ALLOC_WRAPPER_EXTEND_HPP

/**********************************************************/
//standard
#include <cstdlib>
#include <pthread.h>
#include <list>
#include <string>
#include "WrapperCAlloc.hpp"
#include <cuda.h>

namespace MALT
{

//function pointer types
typedef cudaError_enum (*CudaMallocPtr)(void ** devPtr, size_t  size);
typedef cudaError_enum (*CudaFreePtr)(void * devPtr);

//instrument
cudaError_enum malt_wrap_cuda_malloc(void ** devPtr, size_t size, CudaMallocPtr real_cuda_malloc, void * retaddr);
cudaError_enum malt_wrap_cuda_free(void * devPtr, CudaFreePtr real_cuda_free, void * retaddr);

}

#endif //MALT_GPU_CUDA_ALLOC_WRAPPER_EXTEND_HPP
