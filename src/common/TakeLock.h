#ifndef TAKE_LOCK_H
#define TAKE_LOCK_H

/********************  HEADERS  *********************/
#include <cassert>

/********************  MACRO  ***********************/
#define OPTIONAL_CRITICAL(lock,test) do { TakeLock<typeof(lock)> _local_take_lock__(&lock,test);
#define START_CRITICAL(lock) { TakeLock<typeof(lock)> _local_take_lock__(&lock);
#define END_CRITICAL }while(0);

/*********************  CLASS  **********************/
template <class T>
class TakeLock
{
	public:
		TakeLock(T * lock,bool ignore = false);
		~TakeLock(void);
		void unlock(void);
		bool isLock(void);
	private:
		T * lock;
};

/*******************  FUNCTION  *********************/
template <class T>
TakeLock<T>::TakeLock(T* lock, bool ignore)
{
	if (ignore)
	{
		this->lock = NULL;
	} else {
		assert(lock != NULL);
		lock->lock();
	}
}

/*******************  FUNCTION  *********************/
template <class T>
TakeLock<T>::~TakeLock(void )
{
	unlock();
}

/*******************  FUNCTION  *********************/
template <class T>
inline bool TakeLock<T>::isLock(void )
{
	return lock != NULL;
}

/*******************  FUNCTION  *********************/
template <class T>
inline void TakeLock<T>::unlock(void )
{
	if (lock != NULL)
	{
		lock->unlock();
		this->lock = NULL;
	}
}

#endif //TAKE_LOCK_H
