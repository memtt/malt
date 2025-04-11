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
#include "../GraphGenerator.hpp"

/**********************************************************/
using namespace MALTReader;

/**********************************************************/
const std::string CST_DOT_1 = "\
digraph A {\n\
        A -> {B C}\n\
}\n\
";

/**********************************************************/
TEST(TestGraphGenerator, isDotInstalled)
{
	ASSERT_TRUE(GraphGenerator::isDotInstalled());
}

/**********************************************************/
TEST(TestGraphGenerator, convertDotToSvg)
{
	ASSERT_TRUE(GraphGenerator::convertDotToSvg(CST_DOT_1).find("<svg ") != -1);
}
