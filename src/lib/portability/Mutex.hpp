/*****************************************************
             PROJECT  : MALT
             VERSION  : 1.2.2
             DATE     : 06/2023
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MALT_MUTEX_HPP
#define MALT_MUTEX_HPP

/********************  HEADERS  *********************/
#include <common/TakeLock.hpp>
#include <config.h>

/*********************  TYPES  **********************/
#if defined(MALT_PORTABILITY_MUTEX_PTHREAD)
	//pthread mode
	#include "MutexPthread.hpp"
	
	//map macros to generic names
	#define MALT_STATIC_MUTEX_INIT {PTHREAD_MUTEX_INITIALIZER}

	//map types to generic names
	namespace MALT
	{
		typedef StaticMutexPthread StaticMutex;
		typedef MutexPthread Mutex;
	}
#elif defined(MALT_PORTABILITY_MUTEX_DUMMY)
	//dummy mode (not thread safe, only for quik portability)
	#include "MutexDummy.hpp"
	
	//map macros to generic names
	#warning Need to cleanup this
	static MALT::MutexDummy __malt__static_mutex_init__;
	#define MALT_STATIC_MUTEX_INIT __malt__static_mutex_init__
	
	//show some warning
	#warning Caution, you are using the DUMMY mutex implementation, MALT will not be thread-safe !

	//map types to generic names
	namespace MALT
	{
		typedef LockDummy StaticMutex;
		typedef LockDummy Mutex;
	};
#else
	//not found, fail to compile
	#error "No available implementation for mutex, please check definition of one of MALT_PORTABILITY_MUTEX_* macro in config.h or PORTABILITY_MUTEX given to cmake."
#endif

#endif //MALT_MUTEX_HPP
