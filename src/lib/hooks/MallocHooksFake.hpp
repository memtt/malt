/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MATT_MALLOC_HOOKS_FAKE_HPP
#define MATT_MALLOC_HOOKS_FAKE_HPP

/********************  HEADERS  *********************/
#include "MallocHooks.hpp"

/***************** USING NAMESPACE ******************/
namespace MATT
{

/*********************  CLASS  **********************/
class MallocHooksFake : public MallocHooks
{
	public:
		virtual ~MallocHooksFake(void) {};
		virtual void onMalloc(void* ret,size_t size);
		virtual void onFree(void * ret);
		virtual void onCalloc(void * ret,size_t nmemb,size_t size);
		virtual void onRealloc(void * ret,void * ptr, size_t size);
		virtual void onPosixMemalign(int ret,void ** memptr,size_t align, size_t size);
		virtual void onAlignedAlloc(void * ret,size_t alignment, size_t size);
		virtual void onMemalign(void * ret,size_t alignment, size_t size);
		virtual void onValloc(void * ret,size_t size);
		virtual void onPvalloc(void * ret,size_t size);
};

};

#endif //MATT_MALLOC_HOOKS_FAKE_HPP