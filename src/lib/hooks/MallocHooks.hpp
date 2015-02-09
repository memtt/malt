/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MATT_MALLOC_HOOKS_HPP
#define MATT_MALLOC_HOOKS_HPP

/********************  HEADERS  *********************/
#include <cstdlib>

/***************** USING NAMESPACE ******************/
namespace MATT
{

/*********************  CLASS  **********************/
class MallocHooks
{
	public:
		virtual ~MallocHooks(void) {};
		virtual void onMalloc(void* ret,size_t size) = 0;
		virtual void onFree(void * ptr) = 0;
		virtual void onCalloc(void * ret,size_t nmemb,size_t size) = 0;
		virtual void onRealloc(void * ret,void * ptr, size_t size) = 0;
		virtual void onPosixMemalign(int ret,void ** memptr,size_t align, size_t size) = 0;
		virtual void onAlignedAlloc(void * ret,size_t alignment, size_t size) = 0;
		virtual void onMemalign(void * ret,size_t alignment, size_t size) = 0;
		virtual void onValloc(void * ret,size_t size) = 0;
		virtual void onPvalloc(void * ret,size_t size) = 0;
};

/*******************  FUNCTION  *********************/
MallocHooks * mallocHookInit(void);

};

#endif //MATT_MALLOC_HOOKS_HPP