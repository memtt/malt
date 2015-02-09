/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
//standard
#include <common/Debug.hpp>
#include "MallocHooksFake.hpp"

namespace MATT
{

/*******************  FUNCTION  *********************/
void MallocHooksFake::onMalloc(void* ret, size_t size)
{
	MATT_INFO_ARG("%1 = malloc( %2 )").arg(ret).arg(size).end();
}

/*******************  FUNCTION  *********************/
void MallocHooksFake::onFree(void* ptr)
{
	MATT_INFO_ARG("free( %1 )").arg(ptr);
}

/*******************  FUNCTION  *********************/
void MallocHooksFake::onCalloc(void* ret, size_t nmemb, size_t size)
{
	MATT_INFO_ARG("%1 = calloc( %2 , %3 )").arg(ret).arg(nmemb).arg(size).end();
}

/*******************  FUNCTION  *********************/
void MallocHooksFake::onAlignedAlloc(void* ret, size_t alignment, size_t size)
{
	MATT_INFO_ARG("%1 = aligned_alloc( %2 , %3 )").arg(ret).arg(alignment).arg(size).end();
}

/*******************  FUNCTION  *********************/
void MallocHooksFake::onMemalign(void* ret, size_t alignment, size_t size)
{
	MATT_INFO_ARG("%1 = memalign( %2 , %3 )").arg(ret).arg(alignment).arg(size).end();
}

/*******************  FUNCTION  *********************/
void MallocHooksFake::onPosixMemalign(int ret, void** memptr, size_t align, size_t size)
{
	MATT_INFO_ARG("%1 = posix_memalign( %2 , %3 , %4 )").arg(ret).arg(memptr).arg(align).arg(size).end();
}

/*******************  FUNCTION  *********************/
void MallocHooksFake::onPvalloc(void* ret, size_t size)
{
	MATT_INFO_ARG("%1 = pvalloc( %2 )").arg(ret).arg(size).end();
}

/*******************  FUNCTION  *********************/
void MallocHooksFake::onValloc(void* ret, size_t size)
{
	MATT_INFO_ARG("%1 = valloc( %2 )").arg(ret).arg(size).end();
}

void MallocHooksFake::onRealloc(void* ret, void* ptr, size_t size)
{
	MATT_INFO_ARG("%1 = realloc( %2 , %3 )").arg(ret).arg(ptr).arg(size).end();
}

}
