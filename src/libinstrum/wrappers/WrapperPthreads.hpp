/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 02/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/wrappers/WrapperPthreads.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

#ifndef MALT_WRAPPER_PTHREADS_HPP
#define MALT_WRAPPER_PTHREADS_HPP

/**********************************************************/
//standard
#include <cstdlib>
#include <pthread.h>

namespace MALT
{

/**********************************************************/
/**
 * Structure used to transmit arguments to the intermediate function used
 * to track pthread_create.
**/
struct ThreadTrackerArg
{
	/** User arguement to transmit to the real function provided by user. **/
	void * arg;
	/** Real function provided by user and to call after doing instrumentation. **/
	void *(*routine) (void *);
};

/**********************************************************/
void pthreadWrapperOnExit(void *);
void * pthreadWrapperStartRoutine(void * arg);

}

#endif //MALT_WRAPPER_PTHREADS_HPP

