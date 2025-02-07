/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 02/2025
*    LICENSE  : CeCILL-C
*    FILE     : ./src/lib/wrapper/AllocWrapper.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2014 - 2024
*    AUTHOR   : Sébastien Valat (ECR) - 2014
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
#include "GlobalState.hpp"
#include "AllocWrapperExtend.hpp"

/**********************************************************/
using namespace MALT;

/**********************************************************/
void * MALT::malt_wrap_malloc(size_t size, const MallocFuncPtr & real_malloc, void * retaddr)
{
	//get local TLS and check init
	MALT_WRAPPER_LOCAL_STATE_INIT

	//run the default function
	assert(gblState.status > ALLOC_WRAP_INIT_SYM);
	
	ticks t = Clock::getticks();
	void * res = real_malloc(size);
	t = Clock::getticks() - t;

	//profile
	MALT_WRAPPER_LOCAL_STATE_ACTION(localState.profiler->onMalloc(res,size,t,MALLOC_KIND_MALLOC), retaddr);

	//return segment to user
	return res;
}

/**********************************************************/
void MALT::malt_wrap_free(void * ptr, const FreeFuncPtr & real_free, void * retaddr)
{
	//get local TLS and check init
	MALT_WRAPPER_LOCAL_STATE_INIT;

	//profile
	MALT_WRAPPER_LOCAL_STATE_ACTION(localState.profiler->onFree(ptr,0), retaddr);

	//run the default function
	assert(gblState.status > ALLOC_WRAP_INIT_SYM);
	real_free(ptr);
}

/**********************************************************/
void * MALT::malt_wrap_calloc(size_t nmemb,size_t size, const CallocFuncPtr & real_calloc, void * retaddr)
{
	//get local TLS and check init
	MALT_WRAPPER_LOCAL_STATE_INIT

	//calloc need a special trick for first use due to usage in dlsym
	//this way it avoid to create infinite loop
	if (gblState.status == ALLOC_WRAP_INIT_SYM)
	{
		assert(sizeof(gblCallocIniBuffer) >= size);
		return gblCallocIniBuffer;
	}
	
	//run the default function
	assert(gblState.status > ALLOC_WRAP_INIT_SYM);
	
	ticks t = Clock::getticks();
	void * res = real_calloc(nmemb,size);
	t = Clock::getticks() - t;

	//profile
	MALT_WRAPPER_LOCAL_STATE_ACTION(localState.profiler->onCalloc(res,nmemb,size,t), retaddr);

	//return result to user
	return res;
}

/**********************************************************/
void * MALT::malt_wrap_realloc(void * ptr,size_t size, const ReallocFuncPtr & real_realloc, void * retaddr)
{
	//get local TLS and check init
	MALT_WRAPPER_LOCAL_STATE_INIT

	//run the default function
	assert(gblState.status > ALLOC_WRAP_INIT_SYM);
	
	ticks t = Clock::getticks();
	void * res = real_realloc(ptr,size);
	t = Clock::getticks() - t;
	
	//profile
	MALT_WRAPPER_LOCAL_STATE_ACTION(localState.profiler->onRealloc(ptr,res,size,t), retaddr);
	
	return res;
}

/**********************************************************/
int MALT::malt_wrap_posix_memalign(void **memptr, size_t alignment, size_t size, const PosixMemalignFuncPtr & real_mem_align, void * retaddr)
{
	//get local TLS and check init
	MALT_WRAPPER_LOCAL_STATE_INIT

	//run the default function
	assert(gblState.status > ALLOC_WRAP_INIT_SYM);
	
	ticks t = Clock::getticks();
	int res = real_mem_align(memptr, alignment, size);
	t = Clock::getticks() - t;

	//profile
	MALT_WRAPPER_LOCAL_STATE_ACTION(localState.profiler->onMalloc(*memptr,size,t,MALLOC_KIND_POSIX_MEMALIGN), retaddr);

	//return segment to user
	return res;
}

/**********************************************************/
void * MALT::malt_wrap_aligned_alloc(size_t alignment, size_t size, const AlignedAllocFuncPtr & real_aligned_alloc, void * retaddr)
{
	//get local TLS and check init
	MALT_WRAPPER_LOCAL_STATE_INIT

	//run the default function
	assert(gblState.status > ALLOC_WRAP_INIT_SYM);
	
	ticks t = Clock::getticks();
	void * res = real_aligned_alloc(alignment,size);
	t = Clock::getticks() - t;

	//profile
	MALT_WRAPPER_LOCAL_STATE_ACTION(localState.profiler->onMalloc(res,size,t,MALLOC_KIND_ALIGNED_ALLOC), retaddr);

	//return segment to user
	return res;
}

/**********************************************************/
void *MALT::malt_wrap_memalign(size_t alignment, size_t size, const MemalignFuncPtr & real_memalign, void * retaddr)
{
	//get local TLS and check init
	MALT_WRAPPER_LOCAL_STATE_INIT

	//run the default function
	assert(gblState.status > ALLOC_WRAP_INIT_SYM);
	
	ticks t = Clock::getticks();
	void * res = real_memalign(alignment,size);
	t = Clock::getticks() - t;

	//profile
	MALT_WRAPPER_LOCAL_STATE_ACTION(localState.profiler->onMalloc(res,size,t,MALLOC_KIND_POSIX_MEMALIGN), retaddr);

	//return segment to user
	return res;
}

/**********************************************************/
void * MALT::malt_wrap_valloc(size_t size, const VallocFuncPtr & real_aligned_valloc, void * retaddr)
{
	//get local TLS and check init
	MALT_WRAPPER_LOCAL_STATE_INIT

	//run the default function
	assert(gblState.status > ALLOC_WRAP_INIT_SYM);
	
	ticks t = Clock::getticks();
	void * res = real_aligned_valloc(size);
	t = Clock::getticks() - t;

	//profile
	MALT_WRAPPER_LOCAL_STATE_ACTION(localState.profiler->onMalloc(res,size,t,MALLOC_KIND_VALLOC), retaddr);

	//return segment to user
	return res;
}

/**********************************************************/
void *MALT::malt_wrap_pvalloc(size_t size, const PVallocFuncPtr & real_pvalloc, void * retaddr)
{
	//get local TLS and check init
	MALT_WRAPPER_LOCAL_STATE_INIT

	//run the default function
	assert(gblState.status > ALLOC_WRAP_INIT_SYM);
	
	ticks t = Clock::getticks();
	void * res = real_pvalloc(size);
	t = Clock::getticks() - t;

	//profile
	MALT_WRAPPER_LOCAL_STATE_ACTION(localState.profiler->onMalloc(res,size,t,MALLOC_KIND_PVALLOC), retaddr);

	//return segment to user
	return res;
}

/**********************************************************/
void * MALT::malt_wrap_mmap(void *start,size_t length,int prot,int flags,int fd,off_t offset, const MmapFuncPtr & real_mmap, void * retaddr)
{
	//get local TLS and check init
	MALT_WRAPPER_LOCAL_STATE_INIT

	//run the default function
	assert(gblState.status > ALLOC_WRAP_INIT_SYM);
	void * res = real_mmap(start,length,prot,flags,fd,offset);

	//profile
	//MALT_WRAPPER_LOCAL_STATE_ACTION(localState.profiler->onMmap(res,length,flags,fd));

	//return segment to user
	return res;
}

/**********************************************************/
int MALT::malt_wrap_munmap(void * start, size_t length, const MunmapFuncPtr & real_munmap, void * retaddr)
{
	//get local TLS and check init
	MALT_WRAPPER_LOCAL_STATE_INIT

	//run the default function
	assert(gblState.status > ALLOC_WRAP_INIT_SYM);
	int res = real_munmap(start,length);

	//profile
	//MALT_WRAPPER_LOCAL_STATE_ACTION(localState.profiler->onMalloc(res,size));

	//return segment to user
	return res;
}

/**********************************************************/
void * MALT::malt_wrap_mremap(void *old_address, size_t old_size , size_t new_size, int flags, const MremapFuncPtr & real_mremap, void * retaddr)
{
	//get local TLS and check init
	MALT_WRAPPER_LOCAL_STATE_INIT

	//run the default function
	assert(gblState.status > ALLOC_WRAP_INIT_SYM);
	void * res = real_mremap(old_address,old_size,new_size,flags);

	//profile
	//MALT_WRAPPER_LOCAL_STATE_ACTION(localState.profiler->onMalloc(res,size));

	//return segment to user
	return res;
}
