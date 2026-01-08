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

/**********************************************************/
using namespace MALT;

/**********************************************************/
extern "C" {
	DLL_PUBLIC void __pgi_uacc_cuda_alloc(void ** ptr, void * info, size_t size, long async, int dindex, int error);
	DLL_PUBLIC void __pgi_uacc_cuda_free(void * ptr, long async, int dindex, int error);
}

/**********************************************************/
DLL_PUBLIC void __pgi_uacc_cuda_alloc(void ** ptr, void * info, size_t size, long async, int dindex, int error)
{
	//call wrapper
	malt_wrap_pgi_uacc_cuda_alloc(ptr, info, size, async, dindex, error, gblState.gpuFuncs.pgiUaccCudaAlloc, MALT_RETADDR);
}

/**********************************************************/
DLL_PUBLIC void __pgi_uacc_cuda_free(void * ptr, long async, int dindex, int error)
{
	//call wrapper
	return malt_wrap_pgi_uacc_cuda_free(ptr, async, dindex, error, gblState.gpuFuncs.pgiUaccCudaFree, MALT_RETADDR);
}
