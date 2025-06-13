/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.3.0
*    DATE     : 02/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/wrappers/WrapperPthreads.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (ECR) - 2014
*    AUTHOR   : Sébastien Valat - 2014
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
//standard
#include "WrapperPthreads.hpp"
#include "core/ThreadTracker.hpp"
#include <cstdio>
#include <cassert>
//libc POSIX.1, here we use GNU specific RTLD_NEXT (need _GNU_SOURCE)
#include <dlfcn.h>

namespace MALT
{

/**********************************************************/
/**
 * Function called on thread exit via pthread_key system.
 * Argument is not used, only defined for signature compatibility.
**/
void pthreadWrapperOnExit(void *)
{
	pthread_mutex_lock(&(gblThreadTrackerData.lock));
	//fprintf(stderr,"Destroy thread : %d / %d !\n",gblThreadTrackerData.threadCount,gblThreadTrackerData.maxThreadCount);
	gblThreadTrackerData.threadCount--;
	assert(gblThreadTrackerData.threadCount >= 1);
	pthread_mutex_unlock(&(gblThreadTrackerData.lock));
}

/**********************************************************/
/**
 * Function to be used as wrapper in pthread_create to capture the thread init, update
 * counters and call the real user function.
 * @param arg Muse get a pointer to newly allocated ThreadTrackerArg structure.
**/
void * pthreadWrapperStartRoutine(void * arg)
{
	//update counters
	pthread_mutex_lock(&(gblThreadTrackerData.lock));
	gblThreadTrackerData.threadCount++;
	if (gblThreadTrackerData.threadCount > gblThreadTrackerData.maxThreadCount)
		gblThreadTrackerData.maxThreadCount = gblThreadTrackerData.threadCount;
	pthread_mutex_unlock(&(gblThreadTrackerData.lock));

	//setup the key to get destructor call on thread exit (capture function finish or pthread_exit)
	pthread_setspecific(gblThreadTrackerData.key, (void*)0x1);
	
	//fprintf(stderr,"Create thread : %d / %d !\n",gblThreadTrackerData.threadCount,gblThreadTrackerData.maxThreadCount);
	
	//run child
	ThreadTrackerArg * subarg = (ThreadTrackerArg *)arg;
	void * res = subarg->routine(subarg->arg);
	
	//delete
	delete subarg;
	
	//return
	return res;
}

}
