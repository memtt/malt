/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
//standard
#include <cstdlib>
#include <common/Options.hpp>
#include "Hooks.hpp"

namespace MATT
{

/*******************  FUNCTION  *********************/
/** Function pointer to init function **/
typedef void (InitFuncPtr) (void);
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
/** Prototype of mmap function to get it into function pointer. **/
typedef void *(*MmapFuncPtr)(void *start,size_t length,int prot,int flags,int fd,off_t offset);
/** Prototype of munmap function to get it into function pointer. **/
typedef int (*MunmapFuncPtr)(void *start,size_t length);
/** Prototype of mremap function to get it into function pointer. **/
typedef int (*MremapFuncPtr)(void *old_address, size_t old_size , size_t new_size, int flags);

/********************  ENUM  ************************/
/**
 * Follow the current state of GlobalState structure. By default it start as not initialized
 * (ALLOC_WRAP_NOT_READY) and go to larger values up to ALLOC_WRAP_FINISH).
**/
enum AllocWrapperGlobalStatus
{
	/** 
	 *  Initial state setup by compiler. It geneate lazy init on fist usage (needed to to use of 
	 * GlobalState before entering into main() function). 
	**/
	ALLOC_WRAP_NOT_READY,
	/** Intermediate state used to take care of first calloc() answer due to usage into dlsym() function. **/
	ALLOC_WRAP_INIT_SYM,
	/** Intermediate state when initilazing profiler object. **/
	ALLOC_WRAP_INIT_PROFILER,
	/** Ready for use. **/
	ALLOC_WRAP_READY,
	/** Now finish, ignore future calls to not instrument them after call of atexit(). **/
	ALLOC_WRAP_FINISH
};

/*********************  STRUCT  *********************/
/**
 * Store the global state of alloc wrapper, mainly used to maintain access to the profiling object and
 * keep track of the old (glibc) malloc/calloc... symbols to forward the requests after registration of
 * the event.
 * 
 * CAUTION : the 'state' and 'lock' bariables really need to stay on the top of the structure due to
 * static initialization of global 'gblState' variable.
**/
struct AllocWrapperGlobal
{
	/** Current state in life cycle of the structure. CAUTION, need to stay at first position. **/
	AllocWrapperGlobalStatus status;
	/** 
	 * Mutex to protect the structure in multi-thread mode, only used to protect the init state. 
	 * CAUTION, need to star at second position. 
	**/
	MATT::StaticMutex lock;
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
	MmapFuncPtr mmap;
	/** Pointer to the old (libc) munmap symbol. **/
	MunmapFuncPtr munmap;
	/** Pointer to the old (libc) mremap symbol. **/
	MremapFuncPtr mremap;
	/** Pointer to the profiler (use pointer due to use before main, only way to ensure init before first malloc usage). **/
	GlobalHooks * hooks;
	/** Keep track of user options. **/
	Options * options;
	/** Function used to init the structure on first use. **/
	void init(void);
	/** Function to cleanup the structure at exit. **/
	static void onExit(void);
};

/********************  STRUCT  **********************/
/**
 * Manage the local data attached to each threads. 
 * The content use lazy initialization on first access thanks to the default status.
 * value of then 'init' field.
**/
struct ThreadLocalState
{
	/** 
	 * Local profiler manager to track stacks and cache information before flushing them in the globoal one.
	 * It avoid to take locks every time and eventually permit to get a complete per-thread tracking.
	**/
	PerThreadHooks * hooks;

	/**
	 * Remember the default init state to manage lazy initialization.**/
	AllocWrapperGlobalStatus status;
	
	/** Function used to init the structure on first use. **/
	void init(void);
};

}
