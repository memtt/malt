/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
//standard
#include "ThreadTracker.hpp"
#include <cstdio>
#include <cassert>
//libc POSIX.1, here we use GNU specific RTLD_NEXT (need _GNU_SOURCE)
#include <dlfcn.h>

namespace MATT
{

/********************* GLOBALS **********************/
static ThreadTrackerData gblThreadTrackerData = {1,1,NULL,PTHREAD_MUTEX_INITIALIZER};

/*******************  FUNCTION  *********************/
void pthreadWrapperOnExit(void *)
{
	pthread_mutex_lock(&(gblThreadTrackerData.lock));
	fprintf(stderr,"Destroy thread : %d / %d !\n",gblThreadTrackerData.threadCount,gblThreadTrackerData.maxThreadCount);
	gblThreadTrackerData.threadCount--;
	assert(gblThreadTrackerData.threadCount >= 1);
	pthread_mutex_unlock(&(gblThreadTrackerData.lock));
}

/*******************  FUNCTION  *********************/
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
	
	fprintf(stderr,"Create thread : %d / %d !\n",gblThreadTrackerData.threadCount,gblThreadTrackerData.maxThreadCount);
	
	//run child
	ThreadTrackerArg * subarg = (ThreadTrackerArg *)arg;
	void * res = subarg->routine(subarg->arg);
	
	//delete
	delete subarg;
	
	//return
	return res;
}

/*******************  FUNCTION  *********************/
int ThreadTracker::getMaxThreadCount(void)
{
	return gblThreadTrackerData.maxThreadCount;
}

/*******************  FUNCTION  *********************/
int ThreadTracker::getThreadCount(void)
{
	return gblThreadTrackerData.threadCount;
}

}

/*******************  FUNCTION  *********************/
int pthread_create(pthread_t *thread, const pthread_attr_t *attr,void *(*start_routine) (void *), void *arg)
{
	//init
	if (MATT::gblThreadTrackerData.pthread_create == NULL)
	{
		 MATT::gblThreadTrackerData.pthread_create = (MATT::PthreadCreateFuncPtr)dlsym(RTLD_NEXT,"pthread_create");
		 pthread_key_create(&MATT::gblThreadTrackerData.key,MATT::pthreadWrapperOnExit);
	}
	
	//prepare args
	MATT::ThreadTrackerArg * subarg = new MATT::ThreadTrackerArg;
	subarg->arg = arg;
	subarg->routine = start_routine;

	//call
	return MATT::gblThreadTrackerData.pthread_create(thread,attr,MATT::pthreadWrapperStartRoutine,subarg);
}
