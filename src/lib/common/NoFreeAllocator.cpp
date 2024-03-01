/*****************************************************
             PROJECT  : MATT
             VERSION  : 1.2.2
             DATE     : 06/2023
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
//std C++
#include <cassert>
//intenrals
#include <common/Debug.hpp>
#include <common/Helpers.hpp>
#include <portability/OS.hpp>
//current
#include "NoFreeAllocator.hpp"

/***************** USING NAMESPACE ******************/
using namespace std;
using namespace MALT;

/*******************  NAMESPACE  ********************/
namespace MALTV2
{

/********************  MACRO  ***********************/
/** By default, align on pointer size **/
#define MATT_BASE_ALIGNMENT (sizeof(void*))

/*******************  FUNCTION  *********************/
/** Global instance to be used by all MATT functions. **/
NoFreeAllocator * gblNoFreeAllocator = NULL;
/** Memory to allocate the initial no free allocator. We use this way to get spinlock init correctly **/
char gblNoFreeAllocatorMem[sizeof(NoFreeAllocator)];

/*******************  FUNCTION  *********************/
void doNoFreeAllocatorInit ( void )
{
	if (gblNoFreeAllocator == NULL)
	{
		gblNoFreeAllocator = new(gblNoFreeAllocatorMem) NoFreeAllocator();
		gblNoFreeAllocator->init();
	}
}

/*******************  FUNCTION  *********************/
/**
 * Init function to setup the allocator. We do not use the class constructor
 * as the instance might be init later than the first object. Thanks to the init()
 * approach we explicitly call the init function when the first MATT hook is used.
 * @param threadsafe Enable internal locking to be thread safe.
**/
void NoFreeAllocator::init( bool threadsafe, bool useInitSegment )
{
	//defaut state
	this->cur = NULL;
	this->totalMem = 0;
	this->inUseMem = 0;
	this->threadsafe = true;
	
	//setup default memory
	this->setupNewSegment(useInitSegment);
}

/*******************  FUNCTION  *********************/
/**
 * Setup a new big segment.
**/
void NoFreeAllocator::setupNewSegment( bool useInitSegment )
{
	//request to system
	NoFreeAllocatorSegment * segment;
	
	//here we ask mmap to populate so we know by advance the exact physical
	//memory consumption of MALT to substract it from the rest of the app
	//instead of getting it randomly lower.
	if (useInitSegment)
		segment = (NoFreeAllocatorSegment *)initSegment;
	else
		segment = (NoFreeAllocatorSegment *)MALT::OS::mmap(NO_FREE_ALLOC_SEG_SIZE,true);
	
	//errors
	assumeArg(segment!=NULL,"Failed to request memory from OS : %1 !").argStrErrno().end();
	
	//update stats
	this->totalMem += NO_FREE_ALLOC_SEG_SIZE;
	this->inUseMem += sizeof(NoFreeAllocatorSegment);
	
	//update pointers
	segment->prev = this->cur;
	segment->data = segment+1;
	segment->pos = 0;
	
	//mark as current
	this->cur = segment;
}

/*******************  FUNCTION  *********************/
/**
 * Allocate a new user segment which will never freed.
 * @param size Define the requested size (will be round to MATT_BASE_ALIGNMENT).
 * @return Address of the allocated segment.
**/
void* NoFreeAllocator::allocate(size_t size)
{
	//vars
	void * ret = NULL;

	//round to multiple of pointer size
	if (size % MATT_BASE_ALIGNMENT != 0)
		size += MATT_BASE_ALIGNMENT - size%MATT_BASE_ALIGNMENT;
	
	//check
	MALT_ASSERT(size % MATT_BASE_ALIGNMENT == 0);
	assumeArg(size <= NO_FREE_ALLOC_SEG_SIZE - sizeof(NoFreeAllocatorSegment),
		"No free allocator do not accept requests larger than %1 but get %2 !")
			.arg(NO_FREE_ALLOC_SEG_SIZE - sizeof(NoFreeAllocatorSegment))
			.arg(size)
			.end();;
	
	MALT_OPTIONAL_CRITICAL(lock,threadsafe)
	
		//check need new segment
		if (cur->pos + size >= NO_FREE_ALLOC_SEG_SIZE)
			setupNewSegment();
		
		//compute addr
		ret = (void*)((size_t)cur->data+cur->pos);
		
		//update status
		cur->pos += size;
		this->inUseMem += size;
	
	MALT_END_CRITICAL
	
	//return
	return ret;
}

/*******************  FUNCTION  *********************/
/**
 * @return Return the total used memory (allocator headers + user allocations).
**/
size_t NoFreeAllocator::getInuseMemory ( void )
{
	return inUseMem;
}

/*******************  FUNCTION  *********************/
/**
 * @return Return the maxium authorized allocation size.
**/
size_t NoFreeAllocator::getMaxSize ( void ) const
{
	return NO_FREE_ALLOC_SEG_SIZE - sizeof(NoFreeAllocatorSegment);
}

/*******************  FUNCTION  *********************/
/**
 * @return Return the total memory allocated fomr OS by the allocator.
 * Will be multiple of NO_FREE_ALLOC_SEG_SIZE.
**/
size_t NoFreeAllocator::getTotalMemory ( void )
{
	return totalMem;
}

/*******************  FUNCTION  *********************/
/**
 * @return The unused memory , will be delta between total and in use memory.
**/
size_t NoFreeAllocator::getUnusedMemory ( void )
{
	return totalMem - getInuseMemory();
}

/*******************  FUNCTION  *********************/
/**
 * Print the current memory state of the allocator.
 * @param out Define the output stream to use.
**/
void NoFreeAllocator::printState(std::ostream & out) const
{
	out << "=============================================================== MATT MEMORY =====================================================================" << endl;
	out << "Internal memory : allocated = ";
	Helpers::printValue(out,totalMem,"B");
	out << " , unused = ";
	Helpers::printValue(out,totalMem - inUseMem,"B");
	out << endl;
	out << "=================================================================================================================================================" << endl;
}

}
