/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.3.1
*    DATE     : 02/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/wrappers/tests/TestThreadTracker.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2022
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
#include <gtest/gtest.h>
#include <thread>
#include <core/ThreadTracker.hpp>

/**********************************************************/
using namespace MALT;

/**********************************************************/
TEST(TestThreadTracker, no_threads)
{
	EXPECT_EQ(1, ThreadTracker::getThreadCount());
	EXPECT_EQ(1, ThreadTracker::getMaxThreadCount());
}

/**********************************************************/
TEST(TestThreadTracker, one_thread)
{
	std::thread threads[10];

	for (int i = 0 ; i < 10 ; i++) {
		threads[i] = std::thread([](){
			usleep(10000);
		});
	}

	for (int i = 0 ; i < 10 ; i++)
		threads[i].join();

	EXPECT_EQ(1, ThreadTracker::getThreadCount());
	EXPECT_EQ(11, ThreadTracker::getMaxThreadCount());
}
