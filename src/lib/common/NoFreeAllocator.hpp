/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MATT_NO_FREE_ALLOCATOR_HPP
#define MATT_NO_FREE_ALLOCATOR_HPP

/********************  HEADERS  *********************/
#include <cstdlib>

/*******************  NAMESPACE  ********************/
namespace MATT
{
	
/********************  MACROS  **********************/
#define NO_FREE_ALLOC_SEG_SIZE (2*1024ul*1024ul)

/********************  STRUCT  **********************/
struct NoFreeAllocatorSegment
{
	NoFreeAllocatorSegment * prev;
	void * data;
	size_t pos;
};
	
/*********************  CLASS  **********************/
class NoFreeAllocator
{
	public:
		NoFreeAllocator(void);
		size_t memory(void);
		void * allocate(size_t size);
	private:
		void setupNewSegment(void);
	private:
		size_t memSum;
		NoFreeAllocatorSegment * cur;
};

}

#endif //MATT_NO_FREE_ALLOCATOR_HPP
