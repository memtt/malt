/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 01/2022
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/portability/tests/TestMutex.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2022
***********************************************************/

/**********************************************************/
#include <gtest/gtest.h>
#include "Mutex.hpp"

/**********************************************************/
using namespace MALT;
using namespace testing;

/**********************************************************/
TEST(TestMutex, lock_unlock_basic)
{
	Mutex mutex;
	mutex.lock();
	mutex.unlock();
}

/**********************************************************/
TEST(TestMutex, try_lock)
{
	Mutex mutex;
	ASSERT_TRUE(mutex.tryLock());
	ASSERT_FALSE(mutex.tryLock());
	mutex.unlock();
}

/**********************************************************/
TEST(TestStaticMutex, lock_unlock_basic)
{
	StaticMutex mutex = MALT_STATIC_MUTEX_INIT;
	mutex.lock();
	mutex.unlock();
}

/**********************************************************/
TEST(TestStaticMutex, try_lock)
{
	StaticMutex mutex = MALT_STATIC_MUTEX_INIT;
	ASSERT_TRUE(mutex.tryLock());
	ASSERT_FALSE(mutex.tryLock());
	mutex.unlock();
}

/**********************************************************/
TEST(TestRwLock, lock_unlock_basic)
{
	RwLockPthread rwlock;
	rwlock.lock(RW_LOCK_READ);
	rwlock.unlock();
	rwlock.lock(RW_LOCK_WRITE);
	rwlock.unlock();
}

/**********************************************************/
TEST(TestRwLock, try_lock)
{
	RwLockPthread rwlock;
	
	//seq 1
	ASSERT_TRUE(rwlock.tryLock(RW_LOCK_WRITE));
	ASSERT_FALSE(rwlock.tryLock(RW_LOCK_WRITE));
	ASSERT_FALSE(rwlock.tryLock(RW_LOCK_READ));
	rwlock.unlock();

	//seq 2
	ASSERT_TRUE(rwlock.tryLock(RW_LOCK_READ));
	ASSERT_FALSE(rwlock.tryLock(RW_LOCK_WRITE));
	ASSERT_TRUE(rwlock.tryLock(RW_LOCK_READ));
	rwlock.unlock();
}
