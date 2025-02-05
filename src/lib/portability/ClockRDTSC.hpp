/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 10/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/portability/CompilerGNU.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (ECR) - 2014
*    AUTHOR   : Sébastien Valat - 2024
***********************************************************/

#ifndef MALT_COMPILER_CLOCK_RDTSC_HPP
#define MALT_COMPILER_CLOCK_RDTSC_HPP

/**********************************************************/
//standard
#include <cycle.h>

/**********************************************************/
namespace MALT
{

/**********************************************************/
typedef ::ticks ticks;

/**********************************************************/
class ClockRDTSC
{
	public:
		static inline ticks getticks();
		static void enableLineratTimingsForTests(void);
	private:
		static bool genLinearTimingsForTests;
		static ticks linearClockForTests; 
};

/**********************************************************/
inline ticks ClockRDTSC::getticks()
{
	if (ClockRDTSC::genLinearTimingsForTests) {
		ClockRDTSC::linearClockForTests += 1000;
		return ClockRDTSC::linearClockForTests;
	} else {
		return ::getticks();
	}
}

}

#endif //MALT_COMPILER_CLOCK_RDTSC_HPP
