/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.2
*    DATE     : 06/2023
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/portability/MutexPthread.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (ECR) - 2014
*    AUTHOR   : Sébastien Valat - 2022
***********************************************************/

#ifndef MALT_MUTEX_PTHREAD_HPP
#define MALT_MUTEX_PTHREAD_HPP

/**********************************************************/
#include "pthread.h"

/**********************************************************/
namespace MALT
{

/**********************************************************/
struct StaticMutexPthread
{
	void lock(void);
	void unlock(void);
	void destroy(void);
	bool tryLock(void);
	pthread_mutex_t mutex;
};

/**********************************************************/
class MutexPthread
{
	public:
		MutexPthread(void);
		~MutexPthread(void);
		void lock(void);
		void unlock(void);
		bool tryLock(void);
	private:
		pthread_mutex_t mutex;
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

}

#endif //MALT_MUTEX_PTHREAD_HPP
