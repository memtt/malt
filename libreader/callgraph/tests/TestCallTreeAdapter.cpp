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
#include "../CallTreeAdapter.hpp"

/**********************************************************/
using namespace MALTReader;

/**********************************************************/
TEST(TestCallTreeAdapter, convertRgbStringToHex)
{
	ASSERT_EQ(CallTreeAdapter::convertRgbStringToHex("(8,16,32)"), "#081020");
	ASSERT_EQ(CallTreeAdapter::convertRgbStringToHex("(8,16,32"), "#081020");
	ASSERT_EQ(CallTreeAdapter::convertRgbStringToHex("(8,16"), "#000000");
}
