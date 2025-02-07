/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 02/2025
*    LICENSE  : CeCILL-C
*    FILE     : ./src/lib/wrapper/InjectCAlloc.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
#include "GlobalState.hpp"
#include "AllocWrapperExtend.hpp"

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
	return malt_wrap_malloc(size, gblState.malloc, MALT_RETADDR);
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
	return malt_wrap_free(ptr, gblState.free, MALT_RETADDR);
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
	return malt_wrap_calloc(nmemb, size, gblState.calloc, MALT_RETADDR);
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
	return malt_wrap_realloc(ptr, size, gblState.realloc, MALT_RETADDR);
}

/**********************************************************/
/**
 * Wrapper of the posix_memalign function to capture allocations. The original symbol will be
 * search by dlsym() in AllocWrapperGlobal::init() .
**/
int posix_memalign(void ** memptr,size_t align, size_t size)
{
	return malt_wrap_posix_memalign(memptr, align, size, gblState.posix_memalign, MALT_RETADDR);
}

/**********************************************************/
/**
 * Wrapper of the aligned_alloc function to capture allocations. The original symbol will be
 * search by dlsym() in AllocWrapperGlobal::init() .
**/
void * aligned_alloc(size_t alignment, size_t size)
{
	return malt_wrap_aligned_alloc(alignment, size, gblState.aligned_alloc, MALT_RETADDR);
}

/**********************************************************/
/**
 * Wrapper of the memalign function to capture allocations. The original symbol will be
 * search by dlsym() in AllocWrapperGlobal::init() .
**/
void *memalign(size_t alignment, size_t size)
{
	return malt_wrap_memalign(alignment, size, gblState.memalign, MALT_RETADDR);
}

/**********************************************************/
/**
 * Wrapper of the valloc function to capture allocations. The original symbol will be
 * search by dlsym() in AllocWrapperGlobal::init() .
**/
void *valloc(size_t size)
{
	return malt_wrap_valloc(size, gblState.valloc, MALT_RETADDR);
}

/**********************************************************/
/**
 * Wrapper of the pvalloc function to capture allocations. The original symbol will be
 * search by dlsym() in AllocWrapperGlobal::init() .
**/
void *pvalloc(size_t size)
{
	return malt_wrap_pvalloc(size, gblState.pvalloc, MALT_RETADDR);
}

/**********************************************************/
/**
 * Wrapper of the pvalloc function to capture allocations. The original symbol will be
 * search by dlsym() in AllocWrapperGlobal::init() .
**/
void *mmap(void *start, size_t length, int prot,int flags,int fd, off_t offset)
{
	return malt_wrap_mmap(start, length, prot, flags, fd, offset, gblState.mmap, MALT_RETADDR);
}

/**********************************************************/
/**
 * Wrapper of the pvalloc function to capture allocations. The original symbol will be
 * search by dlsym() in AllocWrapperGlobal::init() .
**/
int munmap(void *start, size_t length)
{
	return malt_wrap_munmap(start, length, gblState.munmap, MALT_RETADDR);
}

/**********************************************************/
/**
 * Wrapper of the pvalloc function to capture allocations. The original symbol will be
 * search by dlsym() in AllocWrapperGlobal::init() .
**/
void * mremap(void *old_address, size_t old_size , size_t new_size, int flags)
{
	return malt_wrap_mremap(old_address, old_size, new_size, flags, gblState.mremap, MALT_RETADDR);
}
