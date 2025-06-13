/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.3.0
*    DATE     : 02/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/core/VmaTracker.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2014
*    AUTHOR   : Sébastien Valat (ECR) - 2014
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

#ifndef MALT_VMA_TRACKER_HPP
#define MALT_VMA_TRACKER_HPP

/**********************************************************/
//standard

/**********************************************************/
namespace MALT
{

/**********************************************************/
struct VmaInfo
{
	size_t start;
	size_t end;
};

/**********************************************************/
/**
 * Class used to track the state of each VMA (Virtual Memory Area) managed by
 * mmap/mremap/munmap. It permit to compute the memory exchange rate with the OS.
 * 
 * @brief Tracking of VMA managed by mmap/mremap/munmap
**/
class VmaTracker
{
	public:
		VmaTracker(void);
		~VmaTracker(void);
		void mmap(void * ptr,size_t size);
		size_t mremap(void * oldPtr,size_t oldSize,void * newPtr,size_t newSize);
		size_t munmap(void * ptr,size_t size);
		std::vector<VmaInfo> getAsVector(void) const;
		size_t getCount(void) const;
		size_t getInsertPosition(void) const;
	public:
		friend std::ostream & operator<<(std::ostream & out, VmaTracker & tracker);
	private:
		void compact(void);
		void grow(void);
	private:
		/** List of available VMA. This array might grow automatically. **/
		VmaInfo * vmas;
		/** Size of the VMA array. **/
		size_t size;
		/** Current active. **/
		size_t count;
		/** Last position where vma is inserted. **/
		size_t lastInsert;
};

}

#endif //ATT_CALL_STACK_INFO_HPP
