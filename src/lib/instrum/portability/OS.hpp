/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 10/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/portability/OS.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (ECR) - 2014
*    AUTHOR   : Sébastien Valat - 2024
***********************************************************/

#ifndef MALT_OS_HPP
#define MALT_OS_HPP

/**********************************************************/
#include <config.h>

/**********************************************************/
#if defined(MALT_PORTABILITY_OS_UNIX)
	//pthread mode
	#include "OSUnix.hpp"

	//map types to generic names
	namespace MALT
	{
		typedef OSUnix OS;
	}
#else
	//not found, fail to compile
	#error "No available implementation for OS, please check definition of one of MALT_PORTABILITY_OS_* macro in config.h or PORTABILITY_OS given to cmake."
#endif

#endif //MALT_OS_HPP
