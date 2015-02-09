/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 02/2015
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MATT_MMAP_HOOKS_FAKE_HPP
#define MATT_MMAP_HOOKS_FAKE_HPP

/********************  HEADERS  *********************/
#include "MmapHooks.hpp"

/***************** USING NAMESPACE ******************/
namespace MATT
{

/*********************  CLASS  **********************/
class MmapHooksFake : public MmapHooks
{
	public:
		virtual ~MmapHooksFake(void) {};
		virtual void onMmap(MmapHooksInfos & info,void * res,void *start, size_t length, int prot,int flags,int fd, size_t offset);
		virtual void onMunmap(MmapHooksInfos & info,int ret,void *start, size_t length);
		virtual void onMremap(MmapHooksInfos & info,void * ret,void *old_address, size_t old_size , size_t new_size, int flags);
		virtual bool mmapCallEnterExit ( void );
		virtual void onMmapEnterFunction ( MmapHooksInfos& info );
		virtual void onMmapExitFunction ( MmapHooksInfos& info );
};

};

#endif //MATT_MMAP_HOOKS_FAKE_HPP