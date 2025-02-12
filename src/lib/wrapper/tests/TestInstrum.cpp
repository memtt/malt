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
void * function_testing_c_malloc()
{
	return malloc(16);
}

/**********************************************************/
TEST(TestInstrum, c_basic_malloc)
{
	
}