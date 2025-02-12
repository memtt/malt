/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 10/2024
*    LICENSE  : CeCILL-C
*    FILE     : ./src/lib/wrapper/ThreadTracker.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2014 - 2024
*    AUTHOR   : Sébastien Valat (ECR) - 2014
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

