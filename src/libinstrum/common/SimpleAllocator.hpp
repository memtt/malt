/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.3.1
*    DATE     : 05/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/common/SimpleAllocator.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (ECR) - 2014
*    AUTHOR   : Sébastien Valat - 2014 - 2018
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

#ifndef MALT_SIMPLE_ALLOCATOR_HPP
#define MALT_SIMPLE_ALLOCATOR_HPP

/**********************************************************/
//standard
#include "config.h"
#include <cstdlib>
#include <iostream>
//portability dependent code
#include <portability/Spinlock.hpp>
#ifdef MALT_ENABLE_INTERNAL_JEMALLOC
	#include "jemalloc-malt.h"
#endif

/**********************************************************/
namespace MALT
{

/**********************************************************/
/** Default memory size to request to system when internal memory is full. **/
#define MALT_ALLOC_SYS_REQ_SIZE (256*1024UL)
/** 
 * Minimal size of allocated chunk (depend on header size for chunk and free chunk handler). 
 * Size is defined as usable size without takking accound of chunk header size.
**/
#define MALT_ALLOC_MIN_SIZE (sizeof(FreeChunk) - sizeof(Chunk))
/** Lower memory alignment considerd by allocator to split unused segements. **/
#define MALT_ALLOC_BASIC_ALIGN (sizeof(void*))
/** Do not split elements smaller than given size. **/
#define MALT_ALLOC_SPLIT_THRESOLD (2*MALT_ALLOC_MIN_SIZE)

/**********************************************************/
#ifndef MALT_ENABLE_INTERNAL_JEMALLOC
	#define MALT_HAVE_INTERNAL_ALLOC
#endif
#ifdef MALT_HAVE_INTERNAL_ALLOC
	/** Wrapper to replace malloc for internal allocations. **/
	#define MALT_MALLOC(x) MALT::gblInternaAlloc->malloc(x)
	/** Wrapper to replace free for internal allocations. **/
	#define MALT_FREE(x) MALT::gblInternaAlloc->free(x)
	/** Wrapper to replace realloc for internal allocations. **/
	#define MALT_REALLOC(x,y) MALT::gblInternaAlloc->realloc((x),(y))
#elif !defined(MALT_USE_STD_MALLOC)
	#define MALT_MALLOC(x) MALT::internal_je_malloc(x)
	#define MALT_FREE(x) MALT::internal_je_free(x)
	#define MALT_REALLOC(x, y) MALT::internal_je_realloc((x), (y))
#else
	#define MALT_MALLOC(x) malloc(x)
	#define MALT_FREE(x) free(x)
	#define MALT_REALLOC(x, y) realloc((x), (y))
#endif

/**********************************************************/
/**
 * Define an allocated memory chunk. It's defined only by its size as we do not consider
 * segment merging for this simple allocator.
 * 
 * @brief Define allocated chunks for internal allocator.
**/
struct Chunk
{
	//functions
	Chunk(size_t size = 0);
	void * getBody(void);
	bool canContain(size_t size);
	Chunk * split(size_t size);
	static Chunk * getFromBody(void * ptr);
	size_t getTotalSize(void);
	
	//vars
	/** Internal size of the allocated chunk, so it will not take in account the size of the chunk structure. **/
	size_t size;
};

/**********************************************************/
/**
 * Define a free chunk which can be placed into a double linked list.
 * It inherit from Chunk structure.
 * 
 * @brief Define allocated chunks for internal allocator.
**/
struct FreeChunk
{
	//functions
	FreeChunk(void);
	void insertNext(Chunk * chunk);
	void removeFromList(void);
	
	//vars
	/** Inherit from chunk structure, CAUTION, must be the first entry of the struct. **/
	Chunk chunk;
	/** Pointer to the next free chunk of himself. **/
	FreeChunk * prev;
	/** Pointer to the previous free chunk of himself. **/
	FreeChunk * next;
};

/**********************************************************/
/**
 * Implement a simple internal allocator to manage segments on a single free list base.
 * It uses mmap as lower function and keep all allocated memory. It also ensure full
 * physical mapping of the mapped memory. Thanks to this componnent we can
 * separate the segments used by our instrumentation tool and follow our internal memory
 * consumption to not account it when tracking the application one.
 * 
 * @brief A short memory allocator for internal allocations.
**/
class SimpleAllocator
{
	public:
		SimpleAllocator(bool threadSafe = false,size_t sysReqSize = MALT_ALLOC_SYS_REQ_SIZE);
		void * malloc(size_t size);
		void free(void * ptr);
		void * realloc(void * old,size_t size);
		char * strdup(const char * str);
		size_t getTotalMemory(void);
		size_t getUnusedMemory(void);
		size_t getInuseMemory(void);
		size_t getMaxSize(void) const;
		void printState(std::ostream & out = std::cerr) const;
	protected:
		void requestSystemMemory(size_t size);
		void touchMemory(void * ptr,size_t size);
		Chunk * getInList(size_t size);
		Chunk * getInCur(size_t size);
		Chunk * getInSys(size_t size);
	private:
		/** Store the free chunk for resuse. **/
		FreeChunk freeList;
		/** Current chunk to split to get new segments. **/
		Chunk * cur;
		/** Use next fit algo. **/
		FreeChunk * curSearchInList;
		/** Size of memory segment to requests to OS via mmap to get more memory. **/
		size_t sysReqSize;
		/** Track the total memory used by this allocator. **/
		size_t totalMemory;
		/** Track the internal (total) unused memory allocated by this allocator. **/
		size_t unusedMemory;
		/** Enable or disable thread locking. **/
		bool threadSafe;
		/** Lock to protected the free list and internal counters. **/
		Spinlock lock;
};

/**********************************************************/
/**
 * Provide a global pointer to allocate memory on our simple allocator.
 * 
 * @b CAUTION, this allocator is used in our intrumentation tool which might be initialized before
 * main, so we cannot rely on static object initialization order. So it must be a pointer.
 * 
 * @TODO Maybe move this as a singleton to auto init.
**/
extern SimpleAllocator * gblInternaAlloc;

/**********************************************************/
void initInternalAlloc(bool threadSafe = true);

/**********************************************************/
void * internal_je_malloc(size_t size);
void internal_je_free(void * ptr);
void * internal_je_realloc(void * ptr, size_t size);

}

#endif //MALT_SIMPLE_ALLOCATOR_HPP
