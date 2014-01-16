#ifndef MUTEX_H
#define MUTEX_H

/********************  HEADERS  *********************/
#include <common/TakeLock.h>

/*********************  TYPES  **********************/
#if defined(PORTABILITY_MUTEX_PTHREAD)
	#include "MutexPthread.h"
	
	#define STATIC_MUTEX_INIT {PTHREAD_MUTEX_INITIALIZER}

	typedef StaticMutexPthread StaticMutex;
	typedef MutexPthread Mutex;
	typedef TakeLock<Mutex> TakeMutexLock;
#else
	#error "No available implementation for mutex, please check definition of one of PORTABILITY_MUTEX_* macro or PORTABILITY_MUTEX given to cmake."
#endif

#endif //MUTEX_H
