/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 11/2014
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/portability/OS.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (ECR) - 2014
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
