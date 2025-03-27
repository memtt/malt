/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 10/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/common/Debug.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (ECR) - 2014
*    AUTHOR   : Sébastien Valat - 2015 - 2024
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
