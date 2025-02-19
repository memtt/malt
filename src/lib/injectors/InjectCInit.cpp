/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 02/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/injectors/InjectCInit.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
#include "state/GlobalState.hpp"
#include "wrappers/WrapperCAlloc.hpp"

/**********************************************************/
using namespace MALT;

/**********************************************************/
void libdestructor(void) __attribute__((destructor (101)));
void libdestructor(void)
{
	AllocWrapperGlobal::onExit();
}
