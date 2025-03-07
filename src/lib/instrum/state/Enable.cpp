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

/**********************************************************/
extern "C" {

/**********************************************************/
int maltInitStatus(void)
{
	return 1;
}

/**********************************************************/
void maltEnable(void)
{
	//env
	MALT::LazyEnv env;
	env.enable();
}

/**********************************************************/
void maltDisable(void)
{
	//env
	MALT::LazyEnv env;
	env.disable();
}

}