/*****************************************************
             PROJECT  : MALT
             VERSION  : 1.2.2
             DATE     : 06/2023
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
#include <gtest/gtest.h>
#include "Mutex.hpp"

/***************** USING NAMESPACE ******************/
using namespace MALT;
using namespace testing;

/*******************  FUNCTION  *********************/
TEST(TestMutex, lock_unlock_basic)
{
	Mutex mutex;
	mutex.lock();
	mutex.unlock();
}

/*******************  FUNCTION  *********************/
TEST(TestMutex, try_lock)
{
	Mutex mutex;
	ASSERT_TRUE(mutex.tryLock());
	ASSERT_FALSE(mutex.tryLock());
	mutex.unlock();
}

/*******************  FUNCTION  *********************/
TEST(TestStaticMutex, lock_unlock_basic)
{
	StaticMutex mutex = MALT_STATIC_MUTEX_INIT;
	mutex.lock();
	mutex.unlock();
}

/*******************  FUNCTION  *********************/
TEST(TestStaticMutex, try_lock)
{
	StaticMutex mutex = MALT_STATIC_MUTEX_INIT;
	ASSERT_TRUE(mutex.tryLock());
	ASSERT_FALSE(mutex.tryLock());
	mutex.unlock();
}
