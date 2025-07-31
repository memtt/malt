/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.3.1
*    DATE     : 04/2025
*    LICENSE  : CeCILL-C
*    FILE     : ./src/reader/libreader/callgraph/tests/TestCallTreeAdapter.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
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
