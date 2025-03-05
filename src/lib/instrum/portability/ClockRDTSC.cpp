/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 10/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/portability/CompilerGNU.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (ECR) - 2014
*    AUTHOR   : Sébastien Valat - 2024
***********************************************************/

/**********************************************************/
//internals
#include "ClockRDTSC.hpp"

/**********************************************************/
using namespace MALT;

/**********************************************************/
bool ClockRDTSC::genLinearTimingsForTests = false;
ticks ClockRDTSC::linearClockForTests = 0;

/**********************************************************/
void ClockRDTSC::enableLineratTimingsForTests(void)
{
	ClockRDTSC::linearClockForTests = 0;
	ClockRDTSC::genLinearTimingsForTests = true;
}
