/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.5
*    DATE     : 02/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/wrapper/ThreadTracker.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2014 - 2024
*    AUTHOR   : Sébastien Valat (ECR) - 2014
***********************************************************/

#ifndef MALT_THREAD_TRACKER_HPP
#define MALT_THREAD_TRACKER_HPP

/**********************************************************/
//standard
#include <cstdlib>
#include <pthread.h>

namespace MALT
{

/**********************************************************/
/**
 * Signature of pthread_create function to override it.
**/
typedef int (*PthreadCreateFuncPtr)(pthread_t *thread, const pthread_attr_t *attr,void *(*start_routine) (void *), void *arg);

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
/**
 * Structure to keep track of the global state monstly to remind active and 
 * maximum thread count.
**/
struct ThreadTrackerData
{
	/** Keep track of alive thread count. **/
	int threadCount;
	/** Keep track of maximum thread count seen since start. **/
	int maxThreadCount;
	/** Pointer to the libc pthread_create method. **/
	PthreadCreateFuncPtr pthread_create;
	/** Mutex to protect update of the two counters. **/
	pthread_mutex_t lock;
	/** Keep track of pthread_key to use its destructor to be notified on thread exit. **/
	pthread_key_t key;
	/** 
	 * Say if thread tracking should be active or not. (not is for exnt time and profile
	 * dumping, not to self intruùent) 
	**/
	bool trackingIsEnabled;
};

/**********************************************************/
/**
 * Put user function in a sub namespace.
**/
struct ThreadTracker
{
	static int getThreadCount(void);
	static int getMaxThreadCount(void);
	static void stopThreadTracking(void);
};

}

#endif //MALT_THREAD_TRACKER_HPP

