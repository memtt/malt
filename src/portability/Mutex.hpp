/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MATT_MUTEX_HPP
#define MATT_MUTEX_HPP

/********************  HEADERS  *********************/
#include <common/TakeLock.hpp>
#include <config.h>

/*********************  TYPES  **********************/
#if defined(MATT_PORTABILITY_MUTEX_PTHREAD)
	//pthread mode
	#include "MutexPthread.hpp"
	
	//map macros to generic names
	#define MATT_STATIC_MUTEX_INIT {PTHREAD_MUTEX_INITIALIZER}

	//map types to generic names
	namespace MATT
	{
		typedef StaticMutexPthread StaticMutex;
		typedef MutexPthread Mutex;
	};
#elif defined(MATT_PORTABILITY_MUTEX_DUMMY)
	//dummy mode (not thread safe, only for quik portability)
	#include "MutexDummy.hpp"
	
	//map macros to generic names
	#warning Need to cleanup this
	static MATT::MutexDummy __matt__static_mutex_init__;
	#define MATT_STATIC_MUTEX_INIT __matt__static_mutex_init__
	
	//show some warning
	#warning Caution, you are using the DUMMY mutex implementation, MATT will not be thread-sage !

	//map types to generic names
	namespace MATT
	{
		typedef MutexDummy StaticMutex;
		typedef MutexDummy Mutex;
	};
#else
	//not found, fail to compile
	#error "No available implementation for mutex, please check definition of one of MATT_PORTABILITY_MUTEX_* macro in config.h or PORTABILITY_MUTEX given to cmake."
#endif

#endif //MATT_MUTEX_HPP
