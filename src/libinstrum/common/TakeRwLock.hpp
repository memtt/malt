/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 11/2014
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/common/TakeLock.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (ECR) - 2014
*    AUTHOR   : Sébastien Valat - 2014
***********************************************************/

#ifndef MALT_TAKE_RW_LOCK_HPP
#define MALT_TAKE_RW_LOCK_HPP

/**********************************************************/
#include <cstdlib>
#include <cassert>

/**********************************************************/
/**
 * This file orginally came from MPC_Allocator_CPP project
 * written under CeCILL-C licence by Sébastien Valat.
**/

/**********************************************************/
/**
 * Simple macro to start an optional critical section by using TakeLock mechanism. It
 * support internal use of return, exceptions... The section must be ended by END_RW_CRITICAL
 * macro. Variables declared in that scope are not visible beyond the END_RW_CRITICAL limit.
 * 
 * @code{c++}
Mutex mylock;
MALT_OPTIONAL_RW_CRITICAL(mylock,RW_LOCK_READ,isParallel)
	//to your stuff
	if (error)
		return -1;
	//final stiff
MALT_END_RW_CRITICAL
 * @endcode
 * 
 * @param lock Define the lock to take. A lock object just has to provide lock() and unlock() methods.
 * @param takeLock Take the lock if true otherwise consider a sequential use and do not take the lock.
**/
#define MALT_OPTIONAL_RW_CRITICAL(lock,mode,takeLock) do { MALT::TakeRwLock<typeof(lock)> _local_take_lock__(&(lock),(mode),(takeLock));

/**********************************************************/
/**
 * Simple macro to start a critical section by using TakeLock mechanism. It
 * support internal use of return, exceptions... The section must be ended by END_RW_CRITICAL
 * macro. Variables declared in that scope are not visible beyond the END_RW_CRITICAL limit.
 * 
 * @code{c++}
Mutex mylock;
MALT_START_CRITICAL(mylock)
	//to your stuff
	if (error)
		return -1;
	//final stiff
MALT_END_CRITICAL
 * @endcode
 * 
 * @param lock Define the lock to take. A lock object just has to provide lock() and unlock() methods.
**/
#define MALT_START_RW_CRITICAL(lock,mode) { MALT::TakeLock<typeof(lock)> _local_take_lock__(&(lock), (mode));

/**********************************************************/
/** Close a critical region defined by START_CRITICAL of OPTIONAL_CRITICAL. **/
#define MALT_END_RW_CRITICAL }while(0);

/**********************************************************/
namespace MALT
{

/**********************************************************/
/**
 * Simple class used to manage locking/unlocking of a lock on a region bases by supported
 * exceptions and return inside it.
 * 
 * This is the concept of RAII which use the capability of an object to be
 * automatically destroyed when going out of its declaration scope. This way exceptions
 * and return are automatically support.
 * 
 * To be supported the lock classes must profive a lock() and unlock() method.
 * 
 * @code{c++}
//... Your non locked code here ...
{
	TakeLock<Mutex> safeAccess(&this->mutex);
	// .... Your locked code here ....
	// Can use safetly use return/throw here, it will unlock automatically.
}
//... You non locked code here...
 * @endcode
 *
 * @brief Wrapper class to implement RAII method for thread locking.
**/
template <class T>
class TakeRwLock
{
	public:
		TakeRwLock(T * lock, RwLockAccess mode,bool takeLock = false);
		~TakeRwLock(void);
		void unlock(void);
		bool isLock(void);
	private:
		/** Pointer to the lock to unlock in destructor, NULL if none. **/
		T * lock;
};

/**********************************************************/
/**
 * Constructor used to take the lock at init time.
 * @param lock Pointer to the luck to use. 
 * @param takeLock Boolean to make the lock optional of not depending on runtime behavior.
**/
template <class T>
TakeRwLock<T>::TakeRwLock(T* lock, RwLockAccess mode, bool takeLock)
{
	if (takeLock)
	{
		assert(lock != NULL);
		lock->lock(mode);
		this->lock = lock;
	} else {
		this->lock = NULL;
	}
}

/**********************************************************/
/**
 * Automatically released the lock when current object (so scope) is destroyed.
**/
template <class T>
TakeRwLock<T>::~TakeRwLock(void )
{
	unlock();
}

/**********************************************************/
/**
 * Short function to check the locking status.
**/
template <class T>
inline bool TakeRwLock<T>::isLock(void )
{
	return lock != NULL;
}

/**********************************************************/
/**
 * Permit to manually unlock inside the context if required.
**/
template <class T>
inline void TakeRwLock<T>::unlock(void )
{
	if (lock != NULL)
	{
		lock->unlock();
		this->lock = NULL;
	}
}

}

#endif //MALT_TAKE_RW_LOCK_HPP

