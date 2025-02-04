/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 10/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/core/VmaTracker.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2014 - 2024
*    AUTHOR   : Sébastien Valat (ECR) - 2014
***********************************************************/

/**********************************************************/
//std
#include <cstring>
//common
#include <common/Debug.hpp>
#include <common/SimpleAllocator.hpp>
//current
#include "VmaTracker.hpp"

/**********************************************************/
#define MALT_VMA_TRACKER_DEFAULT_SIZE 10

/**********************************************************/
namespace MALT
{

/**********************************************************/
VmaTracker::VmaTracker ( void )
{
	this->count = 0;
	this->lastInsert = 0;
	this->size = MALT_VMA_TRACKER_DEFAULT_SIZE;
	this->vmas = (VmaInfo*)MALT_MALLOC(sizeof(VmaInfo)*this->size);
	memset(this->vmas,0,sizeof(VmaInfo)*this->size);
}

/**********************************************************/
VmaTracker::~VmaTracker ( void )
{
	MALT_FREE(this->vmas);
}

/**********************************************************/
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
	this->count = curWrite + 1;
}

/**********************************************************/
void VmaTracker::grow ( void )
{
	assert(this->count == this->size);
	
	size *= 2;
	vmas = (VmaInfo*)MALT_REALLOC(vmas,size);
	memset(vmas+size/2,0,size/2);
}

/**********************************************************/
void VmaTracker::mmap ( void* ptr, size_t size )
{
	//check resize and compact
	if (lastInsert >= this->size)
	{
		if (this->count >= this->size)
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

/**********************************************************/
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

/**********************************************************/
size_t VmaTracker::mremap ( void* oldPtr, size_t oldSize, void* newPtr, size_t newSize )
{
	size_t ret = munmap( oldPtr,oldSize);
	mmap(newPtr,newSize);
	return ret;
}

/**********************************************************/
std::vector<VmaInfo> VmaTracker::getAsVector(void) const
{
	std::vector<VmaInfo> result;
	for (size_t i = 0 ; i < this->count ; i++)
		result.push_back(this->vmas[i]);
	return result;
}

/**********************************************************/
size_t VmaTracker::getCount(void) const
{
	return this->count;
}

/**********************************************************/
size_t VmaTracker::getInsertPosition(void) const
{
	return this->lastInsert;
}

/**********************************************************/
std::ostream & operator<<(std::ostream & out, VmaTracker & tracker)
{
	for (size_t i = 0; i < tracker.count ; i++)
		out << "[" << tracker.vmas[i].start << " - " << tracker.vmas[i].end << "]" << std::endl;
	return out;
}

}
