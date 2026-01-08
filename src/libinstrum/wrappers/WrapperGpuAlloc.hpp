/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 08/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/wrappers/WrapperCAlloc.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2022
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

#ifndef MALT_GPU_ALLOC_WRAPPER_EXTEND_HPP
#define MALT_GPU_ALLOC_WRAPPER_EXTEND_HPP

/**********************************************************/
//standard
#include <cstdlib>
#include <pthread.h>
#include <list>
#include <string>
#include "WrapperCAlloc.hpp"

namespace MALT
{

//function pointer types
typedef void (*PgiUaccCudaAllocPtr)(void **, void *, size_t, long, int, int);
typedef void (*PgiUaccCudaFreePtr)(void *, long, int, int);

//instrument
void malt_wrap_pgi_uacc_cuda_alloc(void ** ptr, void * info, size_t size, long async, int dindex, int error, PgiUaccCudaAllocPtr real_cuda_alloc, void * retaddr);
void malt_wrap_pgi_uacc_cuda_free(void * ptr, long async, int dindex, int error, PgiUaccCudaFreePtr real_cuda_free, void * retaddr);

}

#endif //MALT_GPU_ALLOC_WRAPPER_EXTEND_HPP
