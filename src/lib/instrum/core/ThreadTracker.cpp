/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 10/2024
*    LICENSE  : CeCILL-C
*    FILE     : ./src/lib/instrum/wrapper/ThreadTracker.cpp
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
ThreadTrackerData gblThreadTrackerData = {1,1,NULL,PTHREAD_MUTEX_INITIALIZER, true};

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
