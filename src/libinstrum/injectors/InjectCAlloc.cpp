/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.3.1
*    DATE     : 03/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/injectors/InjectCAlloc.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
#include <sys/mman.h>
#include "state/GlobalState.hpp"
#include "wrappers/WrapperCAlloc.hpp"
#include "portability/Visibility.hpp"
#include "portability/Python.hpp"

/**********************************************************/
using namespace MALT;

/**********************************************************/
/**
 * Wrapper of the malloc function to capture allocations. The original symbol will be
 * search by dlsym() in AllocWrapperGlobal::init() .
 * 
 * @param size Size of the element to allocate.
 * @return Pointer to the allocated memory, NULL in case of error.
**/
DLL_PUBLIC void * malloc(size_t size)
{
	//call wrapper
	return malt_wrap_malloc(size, gblState.allocFuncs.malloc, MALT_RETADDR);
}

/**********************************************************/
/**
 * Wrapper of the free function to capture deallocations. The original symbol will be
 * search by dlsym() in AllocWrapperGlobal::init() .
 * 
 * @param ptr Pointer to the memory segment to released.
**/
DLL_PUBLIC void free(void * ptr)
{
	return malt_wrap_free(ptr, gblState.allocFuncs.free, MALT_RETADDR);
}

/**********************************************************/
/**
 * Wrapper of the calloc function to capture allocations. The original symbol will be
 * search by dlsym() in AllocWrapperGlobal::init() .
 * 
 * @param nmemb Number of element to allocate.
 * @param size Size of the element to allocate.
 * @return Pointer to the allocated memory, NULL in case of error.
**/
DLL_PUBLIC void * calloc(size_t nmemb,size_t size)
{
	return malt_wrap_calloc(nmemb, size, gblState.allocFuncs.calloc, MALT_RETADDR);
}

/**********************************************************/
/**
 * Wrapper of the realloc function to capture allocations. The original symbol will be
 * search by dlsym() in AllocWrapperGlobal::init() .
 * 
 * @param ptr Address of the old segment to resize, NULL to allocate a new one.
 * @param size New requested size, 0 to free the given segment.
 * @return Pointer to the allocated memory, NULL in case of error.
**/
DLL_PUBLIC void * realloc(void * ptr, size_t size)
{
	return malt_wrap_realloc(ptr, size, gblState.allocFuncs.realloc, MALT_RETADDR);
}

/**********************************************************/
/**
 * Wrapper of the posix_memalign function to capture allocations. The original symbol will be
 * search by dlsym() in AllocWrapperGlobal::init() .
**/
DLL_PUBLIC int posix_memalign(void ** memptr,size_t align, size_t size)
{
	return malt_wrap_posix_memalign(memptr, align, size, gblState.allocFuncs.posix_memalign, MALT_RETADDR);
}

/**********************************************************/
/**
 * Wrapper of the aligned_alloc function to capture allocations. The original symbol will be
 * search by dlsym() in AllocWrapperGlobal::init() .
**/
DLL_PUBLIC void * aligned_alloc(size_t alignment, size_t size)
{
	return malt_wrap_aligned_alloc(alignment, size, gblState.allocFuncs.aligned_alloc, MALT_RETADDR);
}

/**********************************************************/
/**
 * Wrapper of the memalign function to capture allocations. The original symbol will be
 * search by dlsym() in AllocWrapperGlobal::init() .
**/
DLL_PUBLIC void *memalign(size_t alignment, size_t size)
{
	return malt_wrap_memalign(alignment, size, gblState.allocFuncs.memalign, MALT_RETADDR);
}

/**********************************************************/
/**
 * Wrapper of the valloc function to capture allocations. The original symbol will be
 * search by dlsym() in AllocWrapperGlobal::init() .
**/
DLL_PUBLIC void *valloc(size_t size)
{
	return malt_wrap_valloc(size, gblState.allocFuncs.valloc, MALT_RETADDR);
}

/**********************************************************/
/**
 * Wrapper of the pvalloc function to capture allocations. The original symbol will be
 * search by dlsym() in AllocWrapperGlobal::init() .
**/
DLL_PUBLIC void *pvalloc(size_t size)
{
	return malt_wrap_pvalloc(size, gblState.allocFuncs.pvalloc, MALT_RETADDR);
}

/**********************************************************/
/**
 * Wrapper of the pvalloc function to capture allocations. The original symbol will be
 * search by dlsym() in AllocWrapperGlobal::init() .
**/
DLL_PUBLIC void *mmap(void *start, size_t length, int prot,int flags,int fd, off_t offset)
{
	return malt_wrap_mmap(start, length, prot, flags, fd, offset, gblState.mmapFuncs.mmap, MALT_RETADDR);
}

/**********************************************************/
/**
 * Wrapper of the pvalloc function to capture allocations. The original symbol will be
 * search by dlsym() in AllocWrapperGlobal::init() .
**/
DLL_PUBLIC int munmap(void *start, size_t length)
{
	return malt_wrap_munmap(start, length, gblState.mmapFuncs.munmap, MALT_RETADDR);
}

/**********************************************************/
/**
 * Wrapper of the pvalloc function to capture allocations. The original symbol will be
 * search by dlsym() in AllocWrapperGlobal::init() .
**/
DLL_PUBLIC void * mremap(void *old_address, size_t old_size , size_t new_size, int flags, void * new_address)
{
	return malt_wrap_mremap(old_address, old_size, new_address, new_size, flags, gblState.mmapFuncs.mremap, MALT_RETADDR);
}
