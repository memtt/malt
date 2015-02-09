/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 02/2015
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MATT_COMPILER_HPP
#define MATT_COMPILER_HPP

/********************  HEADERS  *********************/
#include <config.h>

/*********************  TYPES  **********************/
#if defined(MATT_PORTABILITY_COMPILER_GNU)
	//pthread mode
	#include "CompilerGNU.hpp"

	//map types to generic names
	namespace MATT
	{
		typedef CompilerGNU Compiler;
	}
#else
	//not found, fail to compile
	#error "No available implementation for Compiler, please check definition of one of MATT_PORTABILITY_COMPILER_* macro in config.h or PORTABILITY_COMPILER given to cmake."
#endif

#endif //MATT_COMPILER_HPP
