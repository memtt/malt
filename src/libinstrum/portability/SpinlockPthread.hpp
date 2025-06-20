/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.3.1
*    DATE     : 11/2014
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/portability/SpinlockPthread.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (ECR) - 2014
***********************************************************/

#ifndef MALT_SPINLOCK_PTHREAD_HPP
#define MALT_SPINLOCK_PTHREAD_HPP

/**********************************************************/
#include "pthread.h"

/**********************************************************/
namespace MALT
{

/**********************************************************/
class SpinlockPthread
{
	public:
		SpinlockPthread(void);
		~SpinlockPthread(void);
		void lock(void);
		void unlock(void);
		bool tryLock(void);
	private:
		pthread_spinlock_t spinlock;
};

/**********************************************************/
inline SpinlockPthread::SpinlockPthread(void )
{
	pthread_spin_init(&spinlock,PTHREAD_PROCESS_PRIVATE);
}

/**********************************************************/
inline SpinlockPthread::~SpinlockPthread(void )
{
	pthread_spin_destroy(&spinlock);
}

/**********************************************************/
inline void SpinlockPthread::lock(void )
{
	pthread_spin_lock(&spinlock);
}

/**********************************************************/
inline void SpinlockPthread::unlock(void )
{
	pthread_spin_unlock(&spinlock);
}

/**********************************************************/
inline bool SpinlockPthread::tryLock(void )
{
	return pthread_spin_trylock(&spinlock);
}

}

#endif //MALT_SPINLOCK_PTHREAD_HPP
