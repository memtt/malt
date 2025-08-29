/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.3.1
*    DATE     : 02/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/core/VmaTracker.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2014
*    AUTHOR   : Sébastien Valat (ECR) - 2014
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
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
#define MALT_VMA_TRACKER_DEFAULT_SIZE 256

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
}

/**********************************************************/
void VmaTracker::grow ( void )
{
	//check
	assert(this->count == this->size);
	
	//keep track
	size_t oldSize = this->size;

	//incr
	this->size *= 2;
	printf("GROW %zu\n", this->size);

	//realloc
	this->vmas = (VmaInfo*)MALT_REALLOC(vmas,this->size);

	//Reset new part
	for (size_t i = oldSize ; i < this->size ; i++)
		this->vmas[i].start = this->vmas[i].end = 0;
}

/**********************************************************/
ssize_t VmaTracker::mmap ( void* ptr, size_t size )
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

	//return
	return res + size;
}

/**********************************************************/
ssize_t VmaTracker::munmap ( void* ptr, size_t size )
{
	size_t removed = 0;
	size_t start = (size_t)ptr;
	size_t end = start+size;
	//loop on all to find intersects and remove them
	for (size_t i = 0 ; i < lastInsert ; i++)
	{
		//get current
		VmaInfo & vma = vmas[i];

		//overlap left part
		if (start <= vma.start && end > vma.start && end < vma.end) {
			removed += end - vma.start;
			vma.start = end;
		}

		//overlap right part
		if (start > vma.start && start < vma.end && end >= vma.end) {
			removed += vma.end - start;
			vma.end = start;
		}

		//complete remove
		if (start <= vma.start && end >= vma.end) {
			removed += vma.end - vma.start;
			vma.end = vma.start = 0;
			count--;
		}
		//split
		if (start > vma.start && end < vma.end) {
			mmap((void*)end,vma.end-end);
			vma.end = start;
			removed += end-start;
		}
	}
	return -removed;
}

/**********************************************************/
ssize_t VmaTracker::mremap ( void* oldPtr, size_t oldSize, void* newPtr, size_t newSize )
{
	ssize_t deltaMunmap = munmap( oldPtr,oldSize);
	ssize_t deltaMmap = mmap(newPtr,newSize);
	return deltaMmap + deltaMunmap;
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
		out << "[" << tracker.vmas[i].start << " - " << tracker.vmas[i].end << " / " << (tracker.vmas[i].end - tracker.vmas[i].start) << "]" << std::endl;
	return out;
}

}
