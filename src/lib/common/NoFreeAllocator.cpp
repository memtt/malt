/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 02/2015
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
#include "NoFreeAllocator.hpp"
#include <portability/OS.hpp>
#include <cassert>

/***************** USING NAMESPACE ******************/
using namespace std;

/*******************  NAMESPACE  ********************/
namespace MATT
{

/*******************  FUNCTION  *********************/
NoFreeAllocator::NoFreeAllocator(void)
{
	this->cur = NULL;
	this->memSum = 0;
	this->setupNewSegment();
}

/*******************  FUNCTION  *********************/
void NoFreeAllocator::setupNewSegment(void)
{
	NoFreeAllocatorSegment * segment = (NoFreeAllocatorSegment *)OS::mmap(NO_FREE_ALLOC_SEG_SIZE,true);
	this->memSum += NO_FREE_ALLOC_SEG_SIZE;
	segment->prev = cur;
	segment->data = segment;
	segment->pos = sizeof(segment);
	cur = segment;
}

/*******************  FUNCTION  *********************/
void* NoFreeAllocator::allocate(size_t size)
{
	assert(size <= NO_FREE_ALLOC_SEG_SIZE - sizeof(NoFreeAllocatorSegment) );
	if (cur->pos + size >= NO_FREE_ALLOC_SEG_SIZE)
		setupNewSegment();
	void * ret = (void*)((size_t)cur->data+cur->pos);
	cur->pos += size;
	return ret;
}

/*******************  FUNCTION  *********************/
size_t NoFreeAllocator::memory(void)
{
	return memSum;
}

}
