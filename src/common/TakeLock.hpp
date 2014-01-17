#ifndef ATT_TAKE_LOCK_HPP
#define ATT_TAKE_LOCK_HPP

/********************  HEADERS  *********************/
#include <cassert>

/********************  MACRO  ***********************/
/**
 * Simple macro to start an optional critical section by using TakeLock mechanism. It
 * support internal use of return, exceptions... The section must be ended by END_CRITICAL
 * macro. Variables declared in that scope are not visible beyond the END_CRITICAL limit.
 * 
 * @code{c++}
Mutex mylock;
OPTIONAL_CRITICAL(mylock,isParallel)
	//to your stuff
	if (error)
		return -1;
	//final stiff
END_CRITICAL
 * @endcode
 * 
 * @param lock Define the lock to take. A lock object just has to provide lock() and unlock() methods.
 * @param takeLock Take the lock if true otherwise consider a sequential use and do not take the lock.
**/
#define ATT_OPTIONAL_CRITICAL(lock,takeLock) do { ATT::TakeLock<typeof(lock)> _local_take_lock__(&(lock),(takeLock));

/********************  MACROS  **********************/
/**
 * Simple macro to start a critical section by using TakeLock mechanism. It
 * support internal use of return, exceptions... The section must be ended by END_CRITICAL
 * macro. Variables declared in that scope are not visible beyond the END_CRITICAL limit.
 * 
 * @code{c++}
Mutex mylock;
START_CRITICAL(mylock)
	//to your stuff
	if (error)
		return -1;
	//final stiff
END_CRITICAL
 * @endcode
 * 
 * @param lock Define the lock to take. A lock object just has to provide lock() and unlock() methods.
**/
#define ATT_START_CRITICAL(lock) { ATT::TakeLock<typeof(lock)> _local_take_lock__(&(lock));

/********************  MACROS  **********************/
/** Close a critical region defined by START_CRITICAL of OPTIONAL_CRITICAL. **/
#define ATT_END_CRITICAL }while(0);

/*******************  NAMESPACE  ********************/
namespace ATT
{

/*********************  CLASS  **********************/
/**
 * Simple class used to manage locking/unlocking of a lock on a region bases by supported
 * exceptions and return inside it.
 * 
 * This is the concept of XXXXX (put the ref) which use the capability of an object to be
 * automatically destroyed when going out of its declaration scope. This way exceptions
 * and return are automatically support.
 * 
 * To be supported the lock classes must profive a lock() and unlock() method.
**/
template <class T>
class TakeLock
{
	public:
		TakeLock(T * lock,bool takeLock = false);
		~TakeLock(void);
		void unlock(void);
		bool isLock(void);
	private:
		/** Pointer to the lock to unlock in destructor, NULL if none. **/
		T * lock;
};

/*******************  FUNCTION  *********************/
/**
 * Constructor used to take the lock at init time.
 * @param lock Pointer to the luck to use. 
 * @param takeLock Boolean to make the lock optional of not depending on runtime behavior.
**/
template <class T>
TakeLock<T>::TakeLock(T* lock, bool takeLock)
{
	if (takeLock)
	{
		assert(lock != NULL);
		lock->lock();
		this->lock = lock;
	} else {
		this->lock = NULL;
	}
}

/*******************  FUNCTION  *********************/
/**
 * Automatically released the lock when current object (so scope) is destroyed.
**/
template <class T>
TakeLock<T>::~TakeLock(void )
{
	unlock();
}

/*******************  FUNCTION  *********************/
/**
 * Short function to check the locking status.
**/
template <class T>
inline bool TakeLock<T>::isLock(void )
{
	return lock != NULL;
}

/*******************  FUNCTION  *********************/
/**
 * Permit to manually unlock inside the context if required.
**/
template <class T>
inline void TakeLock<T>::unlock(void )
{
	if (lock != NULL)
	{
		lock->unlock();
		this->lock = NULL;
	}
}

};

#endif //ATT_TAKE_LOCK_HPP

