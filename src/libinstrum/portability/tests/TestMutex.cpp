/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.3.1
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
