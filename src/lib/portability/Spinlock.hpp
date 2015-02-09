/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 02/2015
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MATT_SPINLOCK_HPP
#define MATT_SPINLOCK_HPP

/********************  HEADERS  *********************/
#include <common/TakeLock.hpp>
#include <config.h>

/*********************  TYPES  **********************/
#if defined(MATT_PORTABILITY_SPINLOCK_PTHREAD)
	//pthread mode
	#include "SpinlockPthread.hpp"

	//map types to generic names
	namespace MATT
	{
		typedef SpinlockPthread Spinlock;
	}
#elif defined(MATT_PORTABILITY_SPINLOCK_DUMMY)
	//dummy mode (not thread safe, only for quik portability)
	#include "LockDummy.hpp"
	
	//show some warning
	#warning Caution, you are using the DUMMY mutex implementation, MATT will not be thread-safe !

	//map types to generic names
	namespace MATT
	{
		typedef SpinlockDummy Spinlock;
	};
#else
	//not found, fail to compile
	#error "No available implementation for mutex, please check definition of one of MATT_PORTABILITY_SPINLOCK_* macro in config.h or PORTABILITY_SPINLOCK given to cmake."
#endif

#endif //MATT_SPINLOCK_HPP
