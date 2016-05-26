/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 02/2015
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MATT_MALLOC_WRAPPER_HPP
#define MATT_MALLOC_WRAPPER_HPP

/********************  HEADERS  *********************/
//standard
#include <cstdlib>
#include <hooks/MallocHooks.hpp>
#include <portability/Mutex.hpp>

namespace MATT
{

/*******************  FUNCTION  *********************/
/** Prototype of malloc function to get it into function pointer. **/
typedef void * (*MallocFuncPtr) (size_t size);
/** Prototype of free function to get it into function pointer. **/
typedef void   (*FreeFuncPtr)   (void * ptr);
/** Prototype of calloc function to get it into function pointer. **/
typedef void * (*CallocFuncPtr) (size_t nmemb,size_t size);
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

/********************  ENUM  ************************/
/**
 * Follow the current state of GlobalState structure. By default it start as not initialized
 * (ALLOC_WRAP_NOT_READY) and go to larger values up to ALLOC_WRAP_FINISH).
**/
enum AllocWrapperStatus
{
	/** 
	 *  Initial state setup by compiler. It geneate lazy init on fist usage (needed to to use of 
	 * GlobalState before entering into main() function). 
	**/
	ALLOC_WRAP_NOT_INIT,
	/** Intermediate state used to take care of first calloc() answer due to usage into dlsym() function. **/
	ALLOC_WRAP_INIT_SYMBOL,
	/** Intermediate state when initilazing profiler object. **/
	ALLOC_WRAP_INIT_HOOKS,
	/** Ready for use. **/
	ALLOC_WRAP_READY
};

/********************  STRUCT  **********************/
struct MallocLibcFunctions
{
	/** Pointer to the old (glibc) malloc symbol. **/
	MallocFuncPtr malloc;
	/** Pointer to the old (glibc) free symbol. **/
	FreeFuncPtr free;
	/** Pointer to the old (glibc) calloc symbol. **/
	CallocFuncPtr calloc;
	/** Pointer to the old (glibc) realloc symbol. **/
	ReallocFuncPtr realloc;
	/** Pointer to the old (libc) posix_memalign symbol. **/
	PosixMemalignFuncPtr posix_memalign;
	/** Pointer to the old (libc) aligned_alloc symbol. **/
	AlignedAllocFuncPtr aligned_alloc;
	/** Pointer to the old (libc) valloc symbol. **/
	VallocFuncPtr valloc;
	/** Pointer to the old (libc) pvalloc symbol. **/
	PVallocFuncPtr pvalloc;
	/** Pointer to the old (libc) memalign symbol. **/
	MemalignFuncPtr memalign;
	/** Pointer to the old (libc) mmap symbol. **/
};

/********************  STRUCT  **********************/
struct MallocWrapperState
{
	/** Current state in life cycle of the structure. CAUTION, need to stay at first position. **/
	AllocWrapperStatus status;
	/** 
	 * Mutex to protect the structure in multi-thread mode, only used to protect the init state. 
	 * CAUTION, need to star at second position. 
	**/
	StaticMutex lock;
	/** Pointer to original functions **/
	MallocLibcFunctions functions;
	/** Remind enter-exit mode status **/
	bool enterExit;
};

/*******************  FUNCTION  *********************/
void mallocWrapperInit(void);

}

#endif //MATT_MALLOC_WRAPPER_HPP
