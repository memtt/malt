/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MATT_MMAP_HOOKS_HPP
#define MATT_MMAP_HOOKS_HPP

/***************** USING NAMESPACE ******************/
namespace MATT
{

/*********************  CLASS  **********************/
class MmapHooks
{
	public:
		virtual ~MmapHooks(void) {};
		virtual void onMmap(void * res,void *start, size_t length, int prot,int flags,int fd, size_t offset) = 0;
		virtual void onMunmap(int ret,void *start, size_t length) = 0;
		virtual void onMremap(void * ret,void *old_address, size_t old_size , size_t new_size, int flags) = 0;
};

/*******************  FUNCTION  *********************/
MmapHooks * mmapHookInit(void);

};

#endif //MATT_MMAP_HOOKS_HPP