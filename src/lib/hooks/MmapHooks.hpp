/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MATT_MMAP_HOOKS_HPP
#define MATT_MMAP_HOOKS_HPP

/********************  HEADERS  *********************/
#include <cycle.h>

/***************** USING NAMESPACE ******************/
namespace MATT
{

/*********************  STRUCT  *********************/
struct MmapHooksInfos
{
	ticks calltime;
	void * retaddr;
	void * func;
};

/*********************  CLASS  **********************/
class MmapHooks
{
	public:
		virtual ~MmapHooks(void) {};
		virtual void onMmap(MmapHooksInfos & info,void * res,void *start, size_t length, int prot,int flags,int fd, size_t offset) = 0;
		virtual void onMunmap(MmapHooksInfos & info,int ret,void *start, size_t length) = 0;
		virtual void onMremap(MmapHooksInfos & info,void * ret,void *old_address, size_t old_size , size_t new_size, int flags) = 0;
		virtual bool mmapCallEnterExit(void) = 0;
		virtual void onMmapEnterFunction(MmapHooksInfos & info) = 0;
		virtual void onMmapExitFunction(MmapHooksInfos & info) = 0;
};

/*******************  FUNCTION  *********************/
MmapHooks * mmapHookInit(void);

};

#endif //MATT_MMAP_HOOKS_HPP