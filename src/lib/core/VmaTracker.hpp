 /*****************************************************
             PROJECT  : MALT
             VERSION  : 1.2.2
             DATE     : 10/2020
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MALT_VMA_TRACKER_HPP
#define MALT_VMA_TRACKER_HPP

/*******************  FUNCTION  *********************/
//standard

/*******************  NAMESPACE  ********************/
namespace MALT
{

/*********************  STRUCT  *********************/
struct VmaInfo
{
	size_t start;
	size_t end;
};

/*********************  CLASS  **********************/
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
