/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 10/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/portability/Compiler.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (ECR) - 2014
*    AUTHOR   : Sébastien Valat - 2024
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
