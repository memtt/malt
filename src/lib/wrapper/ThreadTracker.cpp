/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 10/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/wrapper/ThreadTracker.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2014 - 2024
*    AUTHOR   : Sébastien Valat (ECR) - 2014
***********************************************************/

/**********************************************************/
//standard
#include "ThreadTracker.hpp"
#include <cstdio>
#include <cassert>
//libc POSIX.1, here we use GNU specific RTLD_NEXT (need _GNU_SOURCE)
#include <dlfcn.h>

namespace MALT
{

/**********************************************************/
/**
 * Static instance of thread tracker data.
**/
static ThreadTrackerData gblThreadTrackerData = {1,1,NULL,PTHREAD_MUTEX_INITIALIZER, true};

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
static void * pthreadWrapperStartRoutine(void * arg)
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

/**********************************************************/
/**
 * @return Return the maximum number of alive thread during program exacution.
**/
int ThreadTracker::getMaxThreadCount(void)
{
	return gblThreadTrackerData.maxThreadCount;
}

/**********************************************************/
/**
 * @return Return the current alive thread count.
**/
int ThreadTracker::getThreadCount(void)
{
	return gblThreadTrackerData.threadCount;
}

/**********************************************************/
void ThreadTracker::stopThreadTracking(void)
{
	gblThreadTrackerData.trackingIsEnabled = false;
}

}

/**********************************************************/
/**
 * Wrapper of thread_create method.
**/
int pthread_create(pthread_t *thread, const pthread_attr_t *attr,void *(*start_routine) (void *), void *arg)
{
	//init
	if (MALT::gblThreadTrackerData.pthread_create == NULL)
	{
		 MALT::gblThreadTrackerData.pthread_create = (MALT::PthreadCreateFuncPtr)dlsym(RTLD_NEXT,"pthread_create");
		 pthread_key_create(&MALT::gblThreadTrackerData.key,MALT::pthreadWrapperOnExit);
	}

	// trivial no instrum
	if (MALT::gblThreadTrackerData.trackingIsEnabled)
	{
		return MALT::gblThreadTrackerData.pthread_create(thread,attr,start_routine,arg);
	}

	//prepare args
	MALT::ThreadTrackerArg * subarg = new MALT::ThreadTrackerArg;
	subarg->arg = arg;
	subarg->routine = start_routine;

	//call
	return MALT::gblThreadTrackerData.pthread_create(thread,attr,MALT::pthreadWrapperStartRoutine,subarg);
}

