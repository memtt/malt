/*****************************************************
             PROJECT  : MALT
             VERSION  : 1.2.2
             DATE     : 06/2023
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MALT_COMPILER_HPP
#define MALT_COMPILER_HPP

/********************  HEADERS  *********************/
#include <config.h>

/*********************  TYPES  **********************/
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
