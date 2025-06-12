/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.6
*    DATE     : 02/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/common/NoFreeAllocator.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2015 - 2022
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**
 * @file NoFreeAllocator.hpp
 * Here we implemented a custom allocator which do not return the memory.
 * As we didn't need to free the memory we can allocate the segments without
 * adding header. It reduce the memory fragmentation and open the door for
 * a faster impelmentation of allocation algorithm. It also permit to get
 * a better view on the no free memory at the end of execution.
**/

#ifndef MALT_NO_FREE_ALLOCATOR_HPP
#define MALT_NO_FREE_ALLOCATOR_HPP

/**********************************************************/
//std C++
#include <cstdlib>
#include <iostream>
//internals
#include <portability/Spinlock.hpp>

/**********************************************************/
namespace MALTV2
{
	
/**********************************************************/
/**
 * This is the maximum allocation size. The allocator will request memory to
 * the OS with mmap with this request size.
**/
#define NO_FREE_ALLOC_SEG_SIZE (2*1024ul*1024ul)

/**********************************************************/
/**
 * Descriptor of internal allocator memory segments requested to the system.
 * Each segment act as a stack by incrementing the last used position untill
 * full.
**/
struct NoFreeAllocatorSegment
{
	/** Pointer to the previous allocated segment or NULL is none. **/
	NoFreeAllocatorSegment * prev;
	/** Pointer to the usable part of the segment. **/
	void * data;
	/** First free position into the segment (starting from this->data). **/
	size_t pos;
	/** To keep alignement on 2*sizeof(void*) **/
	void * unused;
};
	
/**********************************************************/
/**
 * @brief Implement a custom no free allocator.
 * 
 * Implement a fast allocator which do never free the allocated segment so its
 * implemented like a stack system.
**/
class NoFreeAllocator
{
	public:
		void init(bool threadsafe = true, bool useInitSegment = false);
		void * allocate(size_t size);
		size_t getTotalMemory(void);
		size_t getUnusedMemory(void);
		size_t getInuseMemory(void);
		size_t getMaxSize(void) const;
		void printState(std::ostream & out = std::cerr) const;
	private:
		void setupNewSegment(bool useInitSegment = false);
	private:
		/** Keep track of the total memory allocated by the allocator. **/
		size_t totalMem;
		/** Keep track of the total used memory allocated by the allocator. **/
		size_t inUseMem;
		/** Keep track of the current non full segment used by the allocator. **/
		NoFreeAllocatorSegment * cur;
		/** Locks to be thread safe **/
		MALT::Spinlock lock;
		/** Enable locking **/
		bool threadsafe;
		/** first segment to be used to avoid first mmap so infinite loop at init **/
		char initSegment[NO_FREE_ALLOC_SEG_SIZE];
};

/**********************************************************/
/** Static instance of the allocator for use in all MALT routines. **/
extern NoFreeAllocator * gblNoFreeAllocator;

/**********************************************************/
/** Function to init the global static allocator. **/
void doNoFreeAllocatorInit(void);

/**********************************************************/
/** Short wrapper to ease desactivation of this allocator and usage of the standard one **/
#define MALT_NO_FREE_MALLOC(x) gblNoFreeAllocator->allocate(x)
#define MALT_NO_FREE_NEW(x) new (MALT_NO_FREE_MALLOC(sizeof(x))) x

/**********************************************************/
/** Function to use the global allocator **/
template <class T> T * noFreeMalloc(size_t cnt) {return (T*)gblNoFreeAllocator->allocate(sizeof(T)*cnt);}
char * noFreeStrdup(const char * str);

}

#endif //MALT_NO_FREE_ALLOCATOR_HPP
