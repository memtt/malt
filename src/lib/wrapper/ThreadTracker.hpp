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
typedef int (*PthreadCreateFuncPtr)(pthread_t *thread, const pthread_attr_t *attr,void *(*start_routine) (void *), void *arg);

/********************  STRUCT  **********************/
struct ThreadTrackerArg
{
	void * arg;
	void *(*routine) (void *);
};

/********************  STRUCT  **********************/
struct ThreadTrackerData
{
	int threadCount;
	int maxThreadCount;
	PthreadCreateFuncPtr pthread_create;
	pthread_mutex_t lock;
	pthread_key_t key;
};

/*********************  CLASS  **********************/
struct ThreadTracker
{
	static int getThreadCount(void);
	static int getMaxThreadCount(void);
};

}

#endif //MATT_THREAD_TRACKER_HPP
