#ifndef ATT_MUTEX_HPP
#define ATT_MUTEX_HPP

/********************  HEADERS  *********************/
#include <common/TakeLock.hpp>
#include <config.h>

/*********************  TYPES  **********************/
#if defined(ATT_PORTABILITY_MUTEX_PTHREAD)
	//pthread mode
	#include "MutexPthread.hpp"
	
	//map macros to generic names
	#define ATT_STATIC_MUTEX_INIT {PTHREAD_MUTEX_INITIALIZER}

	//map types to generic names
	namespace ATT
	{
		typedef StaticMutexPthread StaticMutex;
		typedef MutexPthread Mutex;
		typedef TakeLock<Mutex> TakeMutexLock;
	};
#else
	//not found, fail to compile
	#error "No available implementation for mutex, please check definition of one of PORTABILITY_MUTEX_* macro in config.h or PORTABILITY_MUTEX given to cmake."
#endif

#endif //ATT_MUTEX_HPP
