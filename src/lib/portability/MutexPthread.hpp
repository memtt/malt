/*****************************************************
             PROJECT  : MALT
             VERSION  : 1.2.2
             DATE     : 06/2023
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MALT_MUTEX_PTHREAD_HPP
#define MALT_MUTEX_PTHREAD_HPP

/********************  HEADERS  *********************/
#include "pthread.h"

/*******************  NAMESPACE  ********************/
namespace MALT
{

/********************  STRUCT  **********************/
struct StaticMutexPthread
{
	void lock(void);
	void unlock(void);
	void destroy(void);
	bool tryLock(void);
	pthread_mutex_t mutex;
};

/*********************  CLASS  **********************/
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

/*******************  FUNCTION  *********************/
inline void StaticMutexPthread::destroy(void )
{
	pthread_mutex_destroy(&mutex);
}

/*******************  FUNCTION  *********************/
inline void StaticMutexPthread::lock(void )
{
	pthread_mutex_lock(&mutex);
}

/*******************  FUNCTION  *********************/
inline void StaticMutexPthread::unlock(void )
{
	pthread_mutex_unlock(&mutex);
}

/*******************  FUNCTION  *********************/
inline bool StaticMutexPthread::tryLock(void)
{
	return pthread_mutex_trylock(&mutex) == 0;
}

/*******************  FUNCTION  *********************/
inline MutexPthread::MutexPthread(void )
{
	pthread_mutex_init(&mutex,NULL);
}

/*******************  FUNCTION  *********************/
inline MutexPthread::~MutexPthread(void )
{
	pthread_mutex_destroy(&mutex);
}

/*******************  FUNCTION  *********************/
inline void MutexPthread::lock(void )
{
	pthread_mutex_lock(&mutex);
}

/*******************  FUNCTION  *********************/
inline void MutexPthread::unlock(void )
{
	pthread_mutex_unlock(&mutex);
}

/*******************  FUNCTION  *********************/
inline bool MutexPthread::tryLock(void )
{
	return pthread_mutex_trylock(&mutex) == 0;
}

}

#endif //MALT_MUTEX_PTHREAD_HPP
