/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 09/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/reader/libreader/callgraph/tests/TestGraphGenerator.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
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
#ifdef HAVE_GRAPHVIZ
TEST(TestGraphGenerator, isDotInstalled)
{
	ASSERT_TRUE(GraphGenerator::isDotInstalled());
}
#endif //HAVE_GRAPHVIZ

/**********************************************************/
#ifdef HAVE_GRAPHVIZ
TEST(TestGraphGenerator, convertDotToSvg)
{
	ASSERT_TRUE(GraphGenerator::convertDotToSvg(CST_DOT_1).find("<svg ") != std::string::npos);
}
#endif //HAVE_GRAPHVIZ