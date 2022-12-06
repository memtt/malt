/*****************************************************
             PROJECT  : MALT
             VERSION  : 1.2.2
             DATE     : 10/2020
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MALT_ALLOC_WRAPPER_EXTEND_HPP
#define MALT_ALLOC_WRAPPER_EXTEND_HPP

/********************  HEADERS  *********************/
//standard
#include <cstdlib>
#include <pthread.h>

extern "C" {

//function pointers
typedef void * (*malt_wrap_malloc_func_ptr) (size_t size);
typedef void   (*malt_wrap_free_func_ptr)   (void * ptr);


//instrument
void * malt_wrap_malloc(size_t size, malt_wrap_malloc_func_ptr real_malloc);
void malt_wrap_free(void * ptr, malt_wrap_free_func_ptr real_free);
void * malt_wrap_malloc(size_t size, malt_wrap_malloc_func_ptr real_malloc);

}

#endif //MALT_ALLOC_WRAPPER_EXTEND_HPP

/** Prototype of calloc function to get it into function pointer. **/

/** Prototype of realloc function to get it into function pointer. **/
typedef void * (*ReallocFuncPtr)(void * ptr,size_t size);
/** Prototype of posix_memalign function to get it into function pointer. **/
typedef int (*PosixMemalignFuncPtr)(void **memptr, size_t alignment, size_t size);
/** Prototype of aligned_alloc function to get it into function pointer. **/
typedef void *(*AlignedAllocFuncPtr)(size_t alignment, size_t size);
/** Prototype of valloc function to get it into function pointer. **/
typedef void *(*VallocFuncPtr)(size_t size);
/** Prototype of memalign function to get it into function pointer. **/
typedef void *(*MemalignFuncPtr)(size_t alignment, size_t size);
/** Prototype of pvalloc function to get it into function pointer. **/
typedef void *(*PVallocFuncPtr)(size_t size);
/** Prototype of mmap function to get it into function pointer. **/
typedef void *(*MmapFuncPtr)(void *start,size_t length,int prot,int flags,int fd,off_t offset);
/** Prototype of munmap function to get it into function pointer. **/
typedef int (*MunmapFuncPtr)(void *start,size_t length);
/** Prototype of mremap function to get it into function pointer. **/
typedef int (*MremapFuncPtr)(void *old_address, size_t old_size , size_t new_size, int flags);