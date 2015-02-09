/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
#include <common/Debug.hpp>
#include "MmapHooksFake.hpp"

/***************** USING NAMESPACE ******************/
namespace MATT
{

/*******************  FUNCTION  *********************/
void MmapHooksFake::onMmap(void* res, void* start, size_t length, int prot, int flags, int fd, size_t offset)
{
	MATT_INFO_ARG("%1 = mmap( %2 , %3 , %4 , %5 , %6 , %7 )")
		.arg(res)
		.arg(start)
		.arg(length)
		.arg(prot)
		.arg(flags)
		.arg(fd)
		.arg(offset)
		.end();
}

/*******************  FUNCTION  *********************/
void MmapHooksFake::onMremap(void* ret, void* old_address, size_t old_size, size_t new_size, int flags)
{
	MATT_INFO_ARG("%1 = mremap( %1 , %2 , %3 , %4 )")
		.arg(ret)
		.arg(old_address)
		.arg(old_size)
		.arg(new_size)
		.arg(flags)
		.end();
}

/*******************  FUNCTION  *********************/
void MmapHooksFake::onMunmap(int ret, void* start, size_t length)
{
	MATT_INFO_ARG("%1 = munmap( %1 , %2 , %3 )")
		.arg(ret)
		.arg(start)
		.arg(length)
		.end();
}

};
