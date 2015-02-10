/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 02/2015
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
		virtual void onMalloc(MallocHooksInfos & info,void* ret,size_t size);
		virtual void onPreFree(MallocHooksInfos& info, void* ptr);
		virtual void onFree(MallocHooksInfos & info,void * ret);
		virtual void onCalloc(MallocHooksInfos & info,void * ret,size_t nmemb,size_t size);
		virtual void onPreRealloc(MallocHooksInfos& info, void* ptr, size_t size);
		virtual void onRealloc(MallocHooksInfos & info,void * ret,void * ptr, size_t size);
		virtual void onPosixMemalign(MallocHooksInfos & info,int ret,void ** memptr,size_t align, size_t size);
		virtual void onAlignedAlloc(MallocHooksInfos & info,void * ret,size_t alignment, size_t size);
		virtual void onMemalign(MallocHooksInfos & info,void * ret,size_t alignment, size_t size);
		virtual void onValloc(MallocHooksInfos & info,void * ret,size_t size);
		virtual void onPvalloc(MallocHooksInfos & info,void * ret,size_t size);
		virtual void onMallocEnterFunction(MallocHooksInfos& info,void * caller,void * function);
		virtual void onMallocExitFunction(MallocHooksInfos& info,void * caller,void * function);
		virtual bool mallocCallEnterExit(void);
};

};

#endif //MATT_MALLOC_HOOKS_FAKE_HPP