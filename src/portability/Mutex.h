#ifndef MUTEX_H
#define MUTEX_H

/********************  HEADERS  *********************/
#include <common/TakeLock.h>
#include <config.h>

/*********************  TYPES  **********************/
#if defined(PORTABILITY_MUTEX_PTHREAD)
	//pthread mode
	#include "MutexPthread.h"
	
	//map macros to generic names
	#define STATIC_MUTEX_INIT {PTHREAD_MUTEX_INITIALIZER}

	//map types to generic names
	typedef StaticMutexPthread StaticMutex;
	typedef MutexPthread Mutex;
	typedef TakeLock<Mutex> TakeMutexLock;
#else
	//not found, fail to compile
	#error "No available implementation for mutex, please check definition of one of PORTABILITY_MUTEX_* macro in config.h or PORTABILITY_MUTEX given to cmake."
#endif

#endif //MUTEX_H
