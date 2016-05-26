/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.2.0
             DATE     : 04/2015
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/*******************  FUNCTION  *********************/
#include <common/Debug.hpp>
//portability issue, used on linux
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
//locals
#include "ProcPageMapReader.hpp"

/********************  MACRO  ***********************/
#define BUFFER_SIZE 1024
#define PAGE_SIZE 4096

/*******************  NAMESPACE  ********************/
namespace MATT
{

/********************  GLOBALS  *********************/
int ProcPageMapReader::fd = 0;
StaticMutex ProcPageMapReader::mutex = MATT_STATIC_MUTEX_INIT;

/*******************  FUNCTION  *********************/
void ProcPageMapReader::init ( void )
{
	//trivial
	if (fd != 0)
		return;
	
	//lock and check
	MATT_START_CRITICAL(mutex)
		if (fd == 0)
		{
			fd = open("/proc/self/pagemap",O_RDONLY);
			if (fd <= 0)
			{
				MATT_WARNING_ARG("Failed to open /proc/self/pagemap : %1").argStrErrno().end();
				fd = -1;
			}
		}
	MATT_END_CRITICAL
}

/*******************  FUNCTION  *********************/
bool ProcPageMapReader::hasProcPagemap ( void )
{
	//init
	if (fd == 0)
		init();
	
	return fd > 0;
}

/*******************  FUNCTION  *********************/
size_t ProcPageMapReader::internalGetPhysicalSize ( void* ptr, size_t size )
{
	//check
	MATT_ASSERT(ptr != NULL);
	MATT_ASSERT(size <= BUFFER_SIZE * PAGE_SIZE);
	MATT_ASSERT(fd > 0);
	
	//count
	int pages = size / PAGE_SIZE;
	int lastPageSize = size - pages * PAGE_SIZE;
	if (lastPageSize > 0)
		pages++;
	
	//seek
	size_t pos = (size_t)ptr / PAGE_SIZE;
	lseek(fd,sizeof(ProcPageMapEntry) * pos,SEEK_SET);
	
	//read
	ProcPageMapEntry entries[BUFFER_SIZE];
	size_t tmp = read(fd,entries,sizeof(ProcPageMapEntry) * pages);
	MATT_ASSERT(tmp == sizeof(ProcPageMapEntry) * pages);
	
	//count physical
	size_t ret = 0;
	for (int i = 0 ; i < pages ; i++)
	{
		if (entries[i].pfn != 0)
			ret += PAGE_SIZE;
	}
	
	//last page not full
	if (lastPageSize > 0 && entries[pages-1].present)
		ret -= PAGE_SIZE - lastPageSize;
	
	return ret;
}

/*******************  FUNCTION  *********************/
size_t ProcPageMapReader::getPhysicalSize ( void* ptr, size_t size )
{
	//vars
	size_t ret = 0;
	
	//init
	if (fd == 0)
		init();
	
	//trivial
	if (ptr == NULL || size == 0)
		return 0;
	if (size < PAGE_SIZE || fd == -1)
		return size;
	
	//lock
	MATT_START_CRITICAL(mutex)
		//loop y splitting
		for (size_t s = 0 ; s < size ; s+=BUFFER_SIZE*PAGE_SIZE)
		{
			if (size - s < BUFFER_SIZE*PAGE_SIZE)
				ret += internalGetPhysicalSize((char*)ptr+s,size - s);
			else
				ret += internalGetPhysicalSize((char*)ptr+s,BUFFER_SIZE*PAGE_SIZE);
		}
	MATT_END_CRITICAL
	
	return ret;
}

}
