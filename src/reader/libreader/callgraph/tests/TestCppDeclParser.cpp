/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 07/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/reader/libreader/callgraph/tests/TestCppDeclParser.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
#include <gtest/gtest.h>
#include "../CppDeclParser.hpp"

/**********************************************************/
using namespace MALTReader;

/**********************************************************/
TEST(TestCppDeclParser, endsWith)
{
	ASSERT_TRUE(CppDeclParser::endsWith("test value", "value"));
	ASSERT_FALSE(CppDeclParser::endsWith("test Value", "value"));
	ASSERT_FALSE(CppDeclParser::endsWith("test valu", "value"));
}

/**********************************************************/
TEST(TestCppDeclParser, getParameterStartIndex)
{
	ASSERT_EQ(12, CppDeclParser::getParameterStartIndex("int function<std::string>"));
	ASSERT_EQ(-1, CppDeclParser::getParameterStartIndex("int function"));
	ASSERT_EQ(-1, CppDeclParser::getParameterStartIndex("std::vector<int> function"));
}

/**********************************************************/
TEST(TestCppDeclParser, parseCppPrototype_1)
{
	FuncDescription descr = CppDeclParser::parseCppPrototype("int function<std::string>(int a)");
	ASSERT_EQ(descr.arguments, "int a");
	ASSERT_EQ(descr.className, "");
	ASSERT_EQ(descr.classParameters, "");
	ASSERT_EQ(descr.full, "int function<std::string>(int a)");
	ASSERT_EQ(descr.funcName, "function");
	ASSERT_EQ(descr.funcParameters, "std::string");
	ASSERT_EQ(descr.isConst, false);
	ASSERT_EQ(descr.nameSpace, "");
	ASSERT_EQ(descr.returnType, "int");
}

/**********************************************************/
TEST(TestCppDeclParser, parseCppPrototype_2)
{
	FuncDescription descr = CppDeclParser::parseCppPrototype("int function(int a)");
	ASSERT_EQ(descr.arguments, "int a");
	ASSERT_EQ(descr.className, "");
	ASSERT_EQ(descr.classParameters, "");
	ASSERT_EQ(descr.full, "int function(int a)");
	ASSERT_EQ(descr.funcName, "function");
	ASSERT_EQ(descr.funcParameters, "");
	ASSERT_EQ(descr.isConst, false);
	ASSERT_EQ(descr.nameSpace, "");
	ASSERT_EQ(descr.returnType, "int");
}

/**********************************************************/
TEST(TestCppDeclParser, parseCppPrototype_3)
{
	FuncDescription descr = CppDeclParser::parseCppPrototype("std::list<int> NS::Class<int>::function<std::string>(std::vector<int> & a)");
	ASSERT_EQ(descr.arguments, "std::vector<int> & a");
	ASSERT_EQ(descr.className, "Class");
	ASSERT_EQ(descr.classParameters, "int");
	ASSERT_EQ(descr.full, "std::list<int> NS::Class<int>::function<std::string>(std::vector<int> & a)");
	ASSERT_EQ(descr.funcName, "function");
	ASSERT_EQ(descr.funcParameters, "std::string");
	ASSERT_EQ(descr.isConst, false);
	ASSERT_EQ(descr.nameSpace, "NS");
	ASSERT_EQ(descr.returnType, "std::list<int>");
}

/**********************************************************/
TEST(TestCppDeclParser, getShortName_1)
{
	FuncDescription descr = CppDeclParser::parseCppPrototype("int function<std::string>(int a)");
	ASSERT_EQ("function<...>(...)", CppDeclParser::getShortName(descr));
}

/**********************************************************/
TEST(TestCppDeclParser, getShortName_2)
{
	FuncDescription descr = CppDeclParser::parseCppPrototype("int function(int a)");
	ASSERT_EQ("function(...)", CppDeclParser::getShortName(descr));
}

/**********************************************************/
TEST(TestCppDeclParser, getShortName_3)
{
	FuncDescription descr = CppDeclParser::parseCppPrototype("std::list<int> NS::Class<int>::function<std::string>(std::vector<int> & a)");
	ASSERT_EQ("NS::Class<...>::function<...>(...)", CppDeclParser::getShortName(descr));
}

/**********************************************************/
TEST(TestCppDeclParser, getShortName_4)
{
	ASSERT_EQ("basic_c_func()", CppDeclParser::getShortName("basic_c_func()"));
	ASSERT_EQ("basic()", CppDeclParser::getShortName("basic()"));
}
