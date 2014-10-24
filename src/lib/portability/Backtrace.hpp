/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MATT_BACKTRACE_HPP
#define MATT_BACKTRACE_HPP

/********************  HEADERS  *********************/
#include <config.h>

/*********************  TYPES  **********************/
#if defined(MATT_PORTABILITY_BACKTRACE_GLIBC)
	//pthread mode
	#include "BacktraceGlibc.hpp"

	//map types to generic names
	namespace MATT
	{
		typedef BacktraceGlibc Backtrace;
	}
#elif defined(MATT_PORTABILITY_BACKTRACE_LIBUNWIND)
	//pthread mode
	#include "BacktraceLibunwind.hpp"

	//map types to generic names
	namespace MATT
	{
		typedef BacktraceLibunwind Backtrace;
	}
#else
	//not found, fail to compile
	#error "No available implementation for Backtrace, please check definition of one of MATT_PORTABILITY_BACKTRACE_* macro in config.h or PORTABILITY_BACKTRACE given to cmake."
#endif

#endif //MATT_BACKTRACE_HPP
