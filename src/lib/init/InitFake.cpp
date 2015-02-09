/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
//standard
#include <hooks/ThreadHooksFake.hpp>
#include <hooks/MallocHooksFake.hpp>
#include <hooks/MmapHooksFake.hpp>
#include <common/Debug.hpp>

namespace MATT
{

/*******************  FUNCTION  *********************/
ThreadHooks * threadHookInit(void)
{
	static ThreadHooksFake gblThreadHooks;
	return &gblThreadHooks;
}

/*******************  FUNCTION  *********************/
MallocHooks * mallocHookInit(void)
{
	static MallocHooksFake gblMallocHooks;
	return &gblMallocHooks;
}

/*******************  FUNCTION  *********************/
MmapHooks * mmapHookInit(void)
{
	static MmapHooksFake gblMmapHooks;
	return &gblMmapHooks;
}

}
