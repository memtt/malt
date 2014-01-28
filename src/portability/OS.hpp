/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MATT_OS_HPP
#define MATT_OS_HPP

/********************  HEADERS  *********************/
#include <config.h>

/*********************  TYPES  **********************/
#if defined(MATT_PORTABILITY_OS_UNIX)
	//pthread mode
	#include "OSUnix.hpp"

	//map types to generic names
	namespace MATT
	{
		typedef OSUnix OS;
	};
#else
	//not found, fail to compile
	#error "No available implementation for OS, please check definition of one of MATT_PORTABILITY_OS_* macro in config.h or PORTABILITY_OS given to cmake."
#endif

#endif //MATT_OS_HPP
