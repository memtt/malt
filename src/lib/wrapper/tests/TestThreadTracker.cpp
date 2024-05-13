/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.2
*    DATE     : 06/2023
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/wrapper/tests/TestThreadTracker.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2022
***********************************************************/
/*****************************************************
             PROJECT  : MALT
             VERSION  : 1.2.2
             DATE     : 06/2023
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
#include <gtest/gtest.h>
#include <thread>
#include <ThreadTracker.hpp>

/***************** USING NAMESPACE ******************/
using namespace MALT;

/*******************  FUNCTION  *********************/
TEST(TestThreadTracker, no_threads)
{
	EXPECT_EQ(1, ThreadTracker::getThreadCount());
	EXPECT_EQ(1, ThreadTracker::getMaxThreadCount());
}

/*******************  FUNCTION  *********************/
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
