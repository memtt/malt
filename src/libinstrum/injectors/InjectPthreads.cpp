/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.3.1
*    DATE     : 03/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/injectors/InjectPthreads.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
//standard
#include "core/ThreadTracker.hpp"
#include "wrappers/WrapperPthreads.hpp"
#include <cstdio>
#include <cassert>
//libc POSIX.1, here we use GNU specific RTLD_NEXT (need _GNU_SOURCE)
#include <dlfcn.h>
//pthreads
#include <pthread.h>
#include "portability/Visibility.hpp"

/**********************************************************/
/**
 * Wrapper of thread_create method.
**/
DLL_PUBLIC int pthread_create(pthread_t *thread, const pthread_attr_t *attr,void *(*start_routine) (void *), void *arg)
{
	//init
	if (MALT::gblThreadTrackerData.pthread_create == NULL)
	{
		MALT::gblThreadTrackerData.pthread_create = (MALT::PthreadCreateFuncPtr)dlsym(RTLD_NEXT,"pthread_create");
		pthread_key_create(&MALT::gblThreadTrackerData.key,MALT::pthreadWrapperOnExit);
	}

	// trivial no instrum
	if (MALT::gblThreadTrackerData.trackingIsEnabled == false)
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
