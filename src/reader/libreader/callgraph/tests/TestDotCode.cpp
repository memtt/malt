/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.3.1
*    DATE     : 04/2025
*    LICENSE  : CeCILL-C
*    FILE     : ./src/reader/libreader/callgraph/tests/TestDotCode.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
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