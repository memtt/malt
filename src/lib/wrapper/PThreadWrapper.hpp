/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 02/2015
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MATT_THREAD_TRACKER_HPP
#define MATT_THREAD_TRACKER_HPP

/********************  HEADERS  *********************/
//standard
#include <cstdlib>
#include <pthread.h>
//lcoals
#include <hooks/ThreadHooks.hpp>

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
struct PThreadWrapperArg
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
struct PthreadWrapperData
{
	/** Pointer to the libc pthread_create method. **/
	PthreadCreateFuncPtr pthread_create;
	/** Keep track of pthread_key to use its destructor to be notified on thread exit. **/
	pthread_key_t key;
};

}

#endif //MATT_THREAD_TRACKER_HPP

