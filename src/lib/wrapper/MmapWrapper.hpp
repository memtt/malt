/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MATT_MMAP_WRAPPER_HPP
#define MATT_MMAP_WRAPPER_HPP

/********************  HEADERS  *********************/
//standard
#include <cstdlib>
#include <hooks/MmapHooks.hpp>
#include <portability/Mutex.hpp>
#include "MallocWrapper.hpp"

namespace MATT
{

/*******************  FUNCTION  *********************/
/** Prototype of mmap function to get it into function pointer. **/
typedef void *(*MmapFuncPtr)(void *start,size_t length,int prot,int flags,int fd,off_t offset);
/** Prototype of munmap function to get it into function pointer. **/
typedef int (*MunmapFuncPtr)(void *start,size_t length);
/** Prototype of mremap function to get it into function pointer. **/
typedef int (*MremapFuncPtr)(void *old_address, size_t old_size , size_t new_size, int flags);

/********************  STRUCT  **********************/
struct MmapLibcFunctions
{
	/** Pointer to the old (libc) mmap symbol. **/
	MmapFuncPtr mmap;
	/** Pointer to the old (libc) munmap symbol. **/
	MunmapFuncPtr munmap;
	/** Pointer to the old (libc) mremap symbol. **/
	MremapFuncPtr mremap;
};

/********************  STRUCT  **********************/
struct MmapWrapperState
{
	/** Current state in life cycle of the structure. CAUTION, need to stay at first position. **/
	AllocWrapperStatus status;
	/** 
	 * Mutex to protect the structure in multi-thread mode, only used to protect the init state. 
	 * CAUTION, need to star at second position. 
	**/
	StaticMutex lock;
	/** Pointer to hooks **/
	MmapHooks * hooks;
	/** Pointer to original functions **/
	MmapLibcFunctions functions;
	/** Remind enter-exit mode status **/
	bool enterExit;
};

/*******************  FUNCTION  *********************/
void mmapWrapperInit(void);

}

#endif //MATT_MMAP_WRAPPER_HPP
