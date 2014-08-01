/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MATT_THREAD_TRACKER_HPP
#define MATT_THREAD_TRACKER_HPP

/********************  HEADERS  *********************/
//standard
#include <cstdlib>
#include <pthread.h>

namespace MATT
{

/*********************  TYPES  **********************/
/**
 * Signature of pthread_create function to override it.
**/
typedef int (*PthreadCreateFuncPtr)(pthread_t *thread, const pthread_attr_t *attr,void *(*start_routine) (void *), void *arg);

/********************  STRUCT  **********************/
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

/********************  STRUCT  **********************/
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
};

/*********************  CLASS  **********************/
/**
 * Put user function in a sub namespace.
**/
struct ThreadTracker
{
	static int getThreadCount(void);
	static int getMaxThreadCount(void);
};

}

#endif //MATT_THREAD_TRACKER_HPP

