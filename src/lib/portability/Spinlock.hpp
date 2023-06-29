/*****************************************************
             PROJECT  : MALT
             VERSION  : 1.2.2
             DATE     : 06/2023
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MALT_SPINLOCK_HPP
#define MALT_SPINLOCK_HPP

/********************  HEADERS  *********************/
#include <common/TakeLock.hpp>
#include <config.h>

/*********************  TYPES  **********************/
#if defined(MALT_PORTABILITY_SPINLOCK_PTHREAD)
	//pthread mode
	#include "SpinlockPthread.hpp"

	//map types to generic names
	namespace MALT
	{
		typedef SpinlockPthread Spinlock;
	}
#elif defined(MALT_PORTABILITY_SPINLOCK_DUMMY)
	//dummy mode (not thread safe, only for quik portability)
	#include "LockDummy.hpp"
	
	//show some warning
	#warning Caution, you are using the DUMMY mutex implementation, MALT will not be thread-safe !

	//map types to generic names
	namespace MALT
	{
		typedef SpinlockDummy Spinlock;
	};
#else
	//not found, fail to compile
	#error "No available implementation for mutex, please check definition of one of MALT_PORTABILITY_SPINLOCK_* macro in config.h or PORTABILITY_SPINLOCK given to cmake."
#endif

#endif //MALT_SPINLOCK_HPP
