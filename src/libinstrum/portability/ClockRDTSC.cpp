/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.3.0
*    DATE     : 02/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/portability/ClockRDTSC.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
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
