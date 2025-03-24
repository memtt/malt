/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 02/2025
*    LICENSE  : CeCILL-C
*    FILE     : ./src/lib/instrum/wrapper/GlobalState.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
#include "LazyEnv.hpp"
#include "portability/Visibility.hpp"

/**********************************************************/
extern "C" {

/**********************************************************/
DLL_PUBLIC int maltInitStatus(void)
{
	return 1;
}

/**********************************************************/
DLL_PUBLIC void maltEnable(void)
{
	//env
	MALT::LazyEnv env;
	env.enable();
}

/**********************************************************/
DLL_PUBLIC void maltDisable(void)
{
	//env
	MALT::LazyEnv env;
	env.disable();
}

}