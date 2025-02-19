/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 02/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/injectors/InjectCAlloc.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
#include "state/GlobalState.hpp"
#include "wrappers/WrapperCAlloc.hpp"

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
void * malloc(size_t size)
{
	return malt_wrap_malloc(size, gblState.allocFuncs.malloc, MALT_RETADDR);
}

/**********************************************************/
/**
 * Wrapper of the free function to capture deallocations. The original symbol will be
 * search by dlsym() in AllocWrapperGlobal::init() .
 * 
 * @param ptr Pointer to the memory segment to released.
**/
void free(void * ptr)
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
void * calloc(size_t nmemb,size_t size)
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
void * realloc(void * ptr, size_t size)
{
	return malt_wrap_realloc(ptr, size, gblState.allocFuncs.realloc, MALT_RETADDR);
}

/**********************************************************/
/**
 * Wrapper of the posix_memalign function to capture allocations. The original symbol will be
 * search by dlsym() in AllocWrapperGlobal::init() .
**/
int posix_memalign(void ** memptr,size_t align, size_t size)
{
	return malt_wrap_posix_memalign(memptr, align, size, gblState.allocFuncs.posix_memalign, MALT_RETADDR);
}

/**********************************************************/
/**
 * Wrapper of the aligned_alloc function to capture allocations. The original symbol will be
 * search by dlsym() in AllocWrapperGlobal::init() .
**/
void * aligned_alloc(size_t alignment, size_t size)
{
	return malt_wrap_aligned_alloc(alignment, size, gblState.allocFuncs.aligned_alloc, MALT_RETADDR);
}

/**********************************************************/
/**
 * Wrapper of the memalign function to capture allocations. The original symbol will be
 * search by dlsym() in AllocWrapperGlobal::init() .
**/
void *memalign(size_t alignment, size_t size)
{
	return malt_wrap_memalign(alignment, size, gblState.allocFuncs.memalign, MALT_RETADDR);
}

/**********************************************************/
/**
 * Wrapper of the valloc function to capture allocations. The original symbol will be
 * search by dlsym() in AllocWrapperGlobal::init() .
**/
void *valloc(size_t size)
{
	return malt_wrap_valloc(size, gblState.allocFuncs.valloc, MALT_RETADDR);
}

/**********************************************************/
/**
 * Wrapper of the pvalloc function to capture allocations. The original symbol will be
 * search by dlsym() in AllocWrapperGlobal::init() .
**/
void *pvalloc(size_t size)
{
	return malt_wrap_pvalloc(size, gblState.allocFuncs.pvalloc, MALT_RETADDR);
}

/**********************************************************/
/**
 * Wrapper of the pvalloc function to capture allocations. The original symbol will be
 * search by dlsym() in AllocWrapperGlobal::init() .
**/
void *mmap(void *start, size_t length, int prot,int flags,int fd, off_t offset)
{
	return malt_wrap_mmap(start, length, prot, flags, fd, offset, gblState.mmapFuncs.mmap, MALT_RETADDR);
}

/**********************************************************/
/**
 * Wrapper of the pvalloc function to capture allocations. The original symbol will be
 * search by dlsym() in AllocWrapperGlobal::init() .
**/
int munmap(void *start, size_t length)
{
	return malt_wrap_munmap(start, length, gblState.mmapFuncs.munmap, MALT_RETADDR);
}

/**********************************************************/
/**
 * Wrapper of the pvalloc function to capture allocations. The original symbol will be
 * search by dlsym() in AllocWrapperGlobal::init() .
**/
void * mremap(void *old_address, size_t old_size , size_t new_size, int flags)
{
	return malt_wrap_mremap(old_address, old_size, new_size, flags, gblState.mmapFuncs.mremap, MALT_RETADDR);
}
