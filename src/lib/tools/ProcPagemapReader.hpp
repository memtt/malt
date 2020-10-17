/*****************************************************
             PROJECT  : MALT
             VERSION  : 1.2.2
             DATE     : 10/2020
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MALT_PROC_PAGEMAP_READER_HPP
#define MALT_PROC_PAGEMAP_READER_HPP

/*******************  FUNCTION  *********************/
//standard
#include <cstdio>
#include <stdint.h>
//portability
#include <portability/Mutex.hpp>

/*******************  NAMESPACE  ********************/
namespace MALT
{

/********************  STRUCT  **********************/
//Infos from https://www.kernel.org/doc/Documentation/vm/pagemap.txt
struct ProcPageMapEntry
{
	uint64_t pfn:55;        // Bits 0-54  page frame number (PFN) if present
	                        // Bits 0-4   swap type if swapped
	                        // Bits 5-54  swap offset if swapped
	unsigned char dirty:1;  // Bit  55    pte is soft-dirty (see Documentation/vm/soft-dirty.txt)
	unsigned char zero:5;   // Bits 56-60 zero
	unsigned char file:1;   // Bit  61    page is file-page or shared-anon
	unsigned char swaped:1; // Bit  62    page swapped
	unsigned char present:1;// Bit  63    page present
};

/*********************  CLASS  **********************/
class ProcPageMapReader
{
	public:
		static size_t getPhysicalSize(void * ptr,size_t size);
		static bool hasProcPagemap(void);
	private:
		static void init(void);
		static size_t internalGetPhysicalSize(void * ptr,size_t size);
	private:
		static int fd;
		static StaticMutex mutex;
};

}

#endif //MALT_PROC_PAGEMAP_READER_HPP
