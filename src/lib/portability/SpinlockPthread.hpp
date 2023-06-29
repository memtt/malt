/*****************************************************
             PROJECT  : MALT
             VERSION  : 1.2.2
             DATE     : 06/2023
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MALT_SPINLOCK_PTHREAD_HPP
#define MALT_SPINLOCK_PTHREAD_HPP

/********************  HEADERS  *********************/
#include "pthread.h"

/*******************  NAMESPACE  ********************/
namespace MALT
{

/*********************  CLASS  **********************/
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

/*******************  FUNCTION  *********************/
inline SpinlockPthread::SpinlockPthread(void )
{
	pthread_spin_init(&spinlock,PTHREAD_PROCESS_PRIVATE);
}

/*******************  FUNCTION  *********************/
inline SpinlockPthread::~SpinlockPthread(void )
{
	pthread_spin_destroy(&spinlock);
}

/*******************  FUNCTION  *********************/
inline void SpinlockPthread::lock(void )
{
	pthread_spin_lock(&spinlock);
}

/*******************  FUNCTION  *********************/
inline void SpinlockPthread::unlock(void )
{
	pthread_spin_unlock(&spinlock);
}

/*******************  FUNCTION  *********************/
inline bool SpinlockPthread::tryLock(void )
{
	return pthread_spin_trylock(&spinlock);
}

}

#endif //MALT_SPINLOCK_PTHREAD_HPP
