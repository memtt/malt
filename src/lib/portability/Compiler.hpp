/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.2
*    DATE     : 09/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/portability/Compiler.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (ECR) - 2014
*    AUTHOR   : Sébastien Valat - 2024
***********************************************************/

#ifndef MALT_COMPILER_HPP
#define MALT_COMPILER_HPP

/**********************************************************/
#include <config.h>

/**********************************************************/
#if defined(MALT_PORTABILITY_COMPILER_GNU)
	//pthread mode
	#include "CompilerGNU.hpp"

	//map types to generic names
	namespace MALT
	{
		typedef CompilerGNU Compiler;
	}
#else
	//not found, fail to compile
	#error "No available implementation for Compiler, please check definition of one of MALT_PORTABILITY_COMPILER_* macro in config.h or PORTABILITY_COMPILER given to cmake."
#endif

#endif //MALT_COMPILER_HPP
