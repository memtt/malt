/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 02/2025
*    LICENSE  : CeCILL-C
*    FILE     : ./src/lib/wrapper/InjectCInit.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
#include "GlobalState.hpp"
#include "AllocWrapperExtend.hpp"

/**********************************************************/
using namespace MALT;

/**********************************************************/
void libdestructor(void) __attribute__((destructor (101)));
void libdestructor(void)
{
	AllocWrapperGlobal::onExit();
}
