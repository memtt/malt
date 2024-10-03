/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 10/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/wrapper/tests/TestThreadTracker.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2022 - 2024
***********************************************************/

/**********************************************************/
#include <gtest/gtest.h>
#include <thread>
#include <ThreadTracker.hpp>

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
			usleep(1000);
		});
	}

	for (int i = 0 ; i < 10 ; i++)
		threads[i].join();

	EXPECT_EQ(1, ThreadTracker::getThreadCount());
	EXPECT_EQ(11, ThreadTracker::getMaxThreadCount());
}
