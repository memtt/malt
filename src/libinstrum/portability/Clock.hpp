/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 02/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/portability/Clock.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

#ifndef MALT_CLOCK_HPP
#define MALT_CLOCK_HPP

/**********************************************************/
#include <config.h>

/**********************************************************/
#if defined(MALT_PORTABILITY_CLOCK_RDTSC)
	//pthread mode
	#include "ClockRDTSC.hpp"

	//map types to generic names
	namespace MALT
	{
		typedef ClockRDTSC Clock;
	}
#else
	//not found, fail to compile
	#error "No available implementation for Clock, please check definition of one of MALT_PORTABILITY_CLOCK_* macro in config.h or PORTABILITY_CLOCK given to cmake."
#endif

#endif //MALT_CLOCK_HPP
