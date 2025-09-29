/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 01/2022
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/portability/MutexPthread.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (ECR) - 2014
*    AUTHOR   : Sébastien Valat - 2022
***********************************************************/

#ifndef MALT_MUTEX_PTHREAD_HPP
#define MALT_MUTEX_PTHREAD_HPP

/**********************************************************/
#include "common/Debug.hpp"
#include "pthread.h"

/**********************************************************/
namespace MALT
{

/**********************************************************/
struct StaticMutexPthread
{
	inline void lock(void);
	inline void unlock(void);
	inline void destroy(void);
	inline bool tryLock(void);
	pthread_mutex_t mutex;
};

/**********************************************************/
class MutexPthread
{
	public:
		inline MutexPthread(void);
		inline ~MutexPthread(void);
		inline void lock(void);
		inline void unlock(void);
		inline bool tryLock(void);
	private:
		pthread_mutex_t mutex;
};

/**********************************************************/
enum RwLockAccess
{
	RW_LOCK_READ,
	RW_LOCK_WRITE
};

/**********************************************************/
class RwLockPthread
{
	public:
		inline RwLockPthread(void);
		inline ~RwLockPthread(void);
		inline void lock(RwLockAccess mode);
		inline void unlock(void);
		inline bool tryLock(RwLockAccess mode);
	private:
		pthread_rwlock_t rwlock;
};

/**********************************************************/
inline void StaticMutexPthread::destroy(void )
{
	pthread_mutex_destroy(&mutex);
}

/**********************************************************/
inline void StaticMutexPthread::lock(void )
{
	pthread_mutex_lock(&mutex);
}

/**********************************************************/
inline void StaticMutexPthread::unlock(void )
{
	pthread_mutex_unlock(&mutex);
}

/**********************************************************/
inline bool StaticMutexPthread::tryLock(void)
{
	return pthread_mutex_trylock(&mutex) == 0;
}

/**********************************************************/
inline MutexPthread::MutexPthread(void )
{
	pthread_mutex_init(&mutex,NULL);
}

/**********************************************************/
inline MutexPthread::~MutexPthread(void )
{
	pthread_mutex_destroy(&mutex);
}

/**********************************************************/
inline void MutexPthread::lock(void )
{
	pthread_mutex_lock(&mutex);
}

/**********************************************************/
inline void MutexPthread::unlock(void )
{
	pthread_mutex_unlock(&mutex);
}

/**********************************************************/
inline bool MutexPthread::tryLock(void )
{
	return pthread_mutex_trylock(&mutex) == 0;
}

/**********************************************************/
inline RwLockPthread::RwLockPthread(void)
{
	int status = pthread_rwlock_init(&this->rwlock, nullptr);
	assert(status == 0);
}

/**********************************************************/
inline RwLockPthread::~RwLockPthread(void)
{
	int status = pthread_rwlock_destroy(&this->rwlock);
	assert(status == 0);
}

/**********************************************************/
inline void RwLockPthread::lock(RwLockAccess mode)
{
	int status = 0;
	switch (mode)
	{
		case RW_LOCK_READ:
			status = pthread_rwlock_rdlock(&this->rwlock);
			break;
		case RW_LOCK_WRITE:
			status = pthread_rwlock_wrlock(&this->rwlock);
			break;
		default:
			MALT_FATAL("Should never reach this line !");
			break;
	}
	assert(status == 0);
}

/**********************************************************/
inline void RwLockPthread::unlock(void)
{
	int status = pthread_rwlock_unlock(&this->rwlock);
	assert(status == 0);
}

/**********************************************************/
inline bool RwLockPthread::tryLock(RwLockAccess mode)
{
	int status = 0;
	switch (mode)
	{
		case RW_LOCK_READ:
			status = pthread_rwlock_tryrdlock(&this->rwlock);
			break;
		case RW_LOCK_WRITE:
			status = pthread_rwlock_trywrlock(&this->rwlock);
			break;
		default:
			MALT_FATAL("Should never reach this line !");
			break;
	}
	return (status == 0);
}

}

#endif //MALT_MUTEX_PTHREAD_HPP
