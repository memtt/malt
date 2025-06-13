/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.3.0
*    DATE     : 11/2014
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/portability/Compiler.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (ECR) - 2014
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
