/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 09/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/wrappers/tests/TestThreadTracker.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2022
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
#include <gtest/gtest.h>
#include <thread>
#include <common/Options.hpp>
#include <common/SimpleAllocator.hpp>
#include <core/ThreadTracker.hpp>
#include <state/GlobalState.hpp>

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

/**********************************************************/
int main(int argc, char ** argv)
{
	//init internal allocator
	gblInternaAlloc = new SimpleAllocator(true);
	gblOptions = new Options;

	//create key
	pthread_key_create(&MALT::gblState.tlsKey,nullptr);
	pthread_key_create(&MALT::gblThreadTrackerData.key,MALT::pthreadWrapperOnExit);
	pthread_setspecific(MALT::gblThreadTrackerData.key, (void*)0x2);

	// This allows the user to override the flag on the command line.
	::testing::InitGoogleTest(&argc, argv);

	//run
	return RUN_ALL_TESTS();
}
