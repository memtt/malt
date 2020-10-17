 /*****************************************************
             PROJECT  : MALT
             VERSION  : 1.2.2
             DATE     : 10/2020
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/*******************  FUNCTION  *********************/
//std
#include <cstring>
//common
#include <common/Debug.hpp>
#include <common/SimpleAllocator.hpp>
//current
#include "VmaTracker.hpp"

/********************  MACRO  ***********************/
#define MALT_VMA_TRACKER_DEFAULT_SIZE 1024

/*******************  NAMESPACE  ********************/
namespace MALT
{

/*******************  FUNCTION  *********************/
VmaTracker::VmaTracker ( void )
{
	this->count = 0;
	this->lastInsert = 0;
	this->size = MALT_VMA_TRACKER_DEFAULT_SIZE;
	this->vmas = (VmaInfo*)MALT_MALLOC(sizeof(VmaInfo)*this->size);
	memset(this->vmas,0,sizeof(VmaInfo)*this->size);
}

/*******************  FUNCTION  *********************/
VmaTracker::~VmaTracker ( void )
{
	MALT_FREE(this->vmas);
}

/*******************  FUNCTION  *********************/
void VmaTracker::compact ( void )
{
	//cursors
	size_t curWrite = 0;
	
	//loop
	for (size_t i = 0 ; i < size ; i++)
	{
		bool hasData = (vmas[i].start > 0);
		if (curWrite < i && hasData)
		{
			vmas[curWrite] = vmas[i];
			vmas[i].end = vmas[i].start = 0;
		}
		if (hasData)
			curWrite++;
	}
	lastInsert = curWrite;
}

/*******************  FUNCTION  *********************/
void VmaTracker::grow ( void )
{
	assert(count == size);
	
	size *= 2;
	vmas = (VmaInfo*)MALT_REALLOC(vmas,size);
	memset(vmas+size/2,0,size/2);
}

/*******************  FUNCTION  *********************/
void VmaTracker::mmap ( void* ptr, size_t size )
{
	//check resize and compact
	if (lastInsert >= size)
	{
		if (count >= size)
			grow();
		compact();
	}

	//ensure unused in debug
	assert(vmas[lastInsert].start == 0);
	
	//ensure no overlap
	size_t res = munmap(ptr,size);
	if (res > 0)
		MALT_WARNING("Caution, found some mmap overlaps, removing the old one...");
	
	//insert
	vmas[lastInsert].start = (size_t)ptr;
	vmas[lastInsert].end = (size_t)ptr+(size_t)size;
	
	//increase
	lastInsert++;
	count++;
}

/*******************  FUNCTION  *********************/
size_t VmaTracker::munmap ( void* ptr, size_t size )
{
	size_t removed = 0;
	size_t start = (size_t)ptr;
	size_t end = start+size;
	//loop on all to find intersects and remove them
	for (size_t i = 0 ; i < lastInsert ; i++)
	{
		VmaInfo & vma = vmas[i];
		//overlap left part
		if (start <= vma.start && end > vma.start && end < vma.end)
		{
			removed += end - vma.start;
			vma.start = end;
		}
		//overlap right part
		if (start > vma.start && start < vma.end && end >= vma.end)
		{
			removed += vma.end - start;
			vma.end = start;
		}
		//complete remove
		if (start <= vma.start && end >= vma.end)
		{
			removed += vma.end - vma.start;
			vma.end = vma.start = 0;
			count--;
		}
		//split
		if (start > vma.start && end < vma.end)
		{
			mmap((void*)end,vma.end-end);
			vma.end = start;
			removed += end-start;
		}
	}
	return removed;
}

/*******************  FUNCTION  *********************/
size_t VmaTracker::mremap ( void* oldPtr, size_t oldSize, void* newPtr, size_t newSize )
{
	size_t ret = munmap( oldPtr,oldSize);
	mmap(newPtr,newSize);
	return ret;
}

}
