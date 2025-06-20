/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.3.1
*    DATE     : 03/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/portability/Python.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

#ifndef MALT_PYTHON_HPP
#define MALT_PYTHON_HPP

/**********************************************************/
#include "config.h"

/**********************************************************/
#ifdef MALT_HAVE_PYTHON
	#ifdef MALT_PORTABILITY_PYTHON_LAZY
		#include "PythonLazy.hpp"
	#elif defined(MALT_PORTABILITY_PYTHON_NATIVE)
		#include "PythonNative.hpp"
	#else
		#error "Invalide MALT_PORTABILIT_MODE !"
	#endif
#else //MALT_HAVE_PYTHON
	#include "PythonFake.hpp"
#endif //MALT_HAVE_PYTHON

#endif //MALT_FAKE_PYTHON_HPP
