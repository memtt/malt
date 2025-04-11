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
#include "../DotCode.hpp"

/**********************************************************/
using namespace MALTReader;

/**********************************************************/
const std::string CST_EXPECTED_1 = "\
node  [name=\"value\"];\n\
edge  [name1=\"value1\"];\n\
A [nodeProp=\"propValue\"];\n\
B [nodeProp=\"propValue\"];\n\
A -> B [edgeProp=\"propValue2\"];\n\
";

/**********************************************************/
TEST(TestDotCode, constructor)
{
	DotCode dot;
}

/**********************************************************/
TEST(TestDotCode, usage)
{
	DotCode dot;
	dot.nodeStyle({{"name", "value"}});
	dot.edgeStyle({{"name1", "value1"}});
	dot.node("A", {{"nodeProp", "propValue"}});
	dot.node("B", {{"nodeProp", "propValue"}});
	dot.edge("A", "B", {{"edgeProp", "propValue2"}});
	ASSERT_EQ(CST_EXPECTED_1, dot.toCode());
}