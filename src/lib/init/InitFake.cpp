/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 02/2015
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
//standard
#include <hooks/ThreadHooksFake.hpp>
#include <hooks/MallocHooksFake.hpp>
#include <hooks/MmapHooksFake.hpp>
#include <hooks/ExitHooksFake.hpp>
#include <hooks/EnterExitFunctionHooksFake.hpp>
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

/*******************  FUNCTION  *********************/
ExitHooks * exitHookInit(void)
{
	static ExitHooksFake gblExitHooks;
	return &gblExitHooks;
}

/*******************  FUNCTION  *********************/
EnterExitFunctionHooks * enterExitFunctionHookInit(void)
{
	static EnterExitFunctionHooksFake gblEnterExitFunctionHooks;
	return &gblEnterExitFunctionHooks;
}

}
