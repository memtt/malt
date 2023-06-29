/*****************************************************
             PROJECT  : MALT
             VERSION  : 1.2.2
             DATE     : 06/2023
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MALT_ALLOC_WRAPPER_EXTEND_HPP
#define MALT_ALLOC_WRAPPER_EXTEND_HPP

/********************  HEADERS  *********************/
//standard
#include <cstdlib>
#include <pthread.h>

namespace MALT
{

//get retaddr
#define MALT_RETADDR (__builtin_extract_return_addr(__builtin_return_address(0)))

//function pointer types
typedef void * (*MallocFuncPtr) (size_t size);
typedef void   (*FreeFuncPtr) (void * ptr);
typedef void * (*CallocFuncPtr) (size_t nmemb,size_t size);
typedef void * (*ReallocFuncPtr)(void * ptr,size_t size);
typedef int (*PosixMemalignFuncPtr)(void **memptr, size_t alignment, size_t size);
typedef void *(*AlignedAllocFuncPtr)(size_t alignment, size_t size);
typedef void *(*VallocFuncPtr)(size_t size);
typedef void *(*MemalignFuncPtr)(size_t alignment, size_t size);
typedef void *(*PVallocFuncPtr)(size_t size);
typedef void *(*MmapFuncPtr)(void *start,size_t length,int prot,int flags,int fd,off_t offset);
typedef int (*MunmapFuncPtr)(void *start,size_t length);
typedef void *(*MremapFuncPtr)(void *old_address, size_t old_size , size_t new_size, int flags);

//instrument
void * malt_wrap_malloc(size_t size, const MallocFuncPtr & real_malloc, void * retaddr);
void malt_wrap_free(void * ptr, const FreeFuncPtr & real_free, void * retaddr);
void * malt_wrap_calloc(size_t nmemb,size_t size, const CallocFuncPtr & real_calloc, void * retaddr);
void * malt_wrap_realloc(void * ptr,size_t size, const ReallocFuncPtr & real_realloc, void * retaddr);
int malt_wrap_posix_memalign(void **memptr, size_t alignment, size_t size, const PosixMemalignFuncPtr & real_mem_align, void * retaddr);
void * malt_wrap_aligned_alloc(size_t alignment, size_t size, const AlignedAllocFuncPtr & real_aligned_alloc, void * retaddr);
void * malt_wrap_valloc(size_t size, const VallocFuncPtr & real_aligned_valloc, void * retaddr);
void * malt_wrap_memalign(size_t alignment, size_t size, const MemalignFuncPtr & real_memalign, void * retaddr);
void * malt_wrap_pvalloc(size_t size, const PVallocFuncPtr & real_pvalloc, void * retaddr);
void * malt_wrap_mmap(void *start,size_t length,int prot,int flags,int fd,off_t offset, const MmapFuncPtr & real_mmap, void * retaddr);
int malt_wrap_munmap(void * start, size_t length, const MunmapFuncPtr & real_munmap, void * retaddr);
void * malt_wrap_mremap(void *old_address, size_t old_size , size_t new_size, int flags, const MremapFuncPtr & real_munmap, void * retaddr);

//load symbols
void malt_wrap_extended_symbols(void);

}

#endif //MALT_ALLOC_WRAPPER_EXTEND_HPP
