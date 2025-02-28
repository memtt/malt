/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 10/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/core/tests/TestAllocTraceFile.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2022 - 2024
***********************************************************/

/**********************************************************/
#include <gtest/gtest.h>
#include "../MaltProfile.hpp"

/**********************************************************/
using namespace MALTReader;

/**********************************************************/
TEST(TestExtractorHelpers, isAllocFunction)
{
	ASSERT_FALSE(ExtractorHelpers::isAllocFunction("main"));
	ASSERT_TRUE(ExtractorHelpers::isAllocFunction("malloc"));
	ASSERT_FALSE(ExtractorHelpers::isAllocFunction("mmalloc"));
}
