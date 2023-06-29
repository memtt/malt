/*****************************************************
             PROJECT  : MALT
             VERSION  : 1.2.2
             DATE     : 06/2023
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MALT_OS_HPP
#define MALT_OS_HPP

/********************  HEADERS  *********************/
#include <config.h>

/*********************  TYPES  **********************/
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
