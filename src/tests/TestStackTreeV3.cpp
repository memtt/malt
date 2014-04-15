/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
#include <gtest/gtest.h>
#include <stack-tree/stack-tree-subset/StackTreeSubSet.hpp>

/***************** USING NAMESPACE ******************/
using namespace MATT;

/********************** CONSTS **********************/
static const char CST_VALUE_1[] = "[{\n\t\"stack\":[\"0xb\", \"0xa\"],\n\t\"infos\":110\n}]";

/*******************  FUNCTION  *********************/
TEST(TestStackTreeSubSet,getNode)
{
	//build a short tree
	StackTreeSubSet<int> tree;
	StackTreeSubSet<int>::Handler handler = tree.buildNewhandler();
	tree.enter(handler,(void*)0xA);
	tree.enter(handler,(void*)0xB);
	
	//try to get values
	int & value1 = tree.getData(handler);
	value1 = 110;
	
	//check if we reuse the value on next call
	StackTreeSubSet<int>::Handler handler2 = tree.buildNewhandler();
	tree.enter(handler2,(void*)0xA);
	tree.enter(handler2,(void*)0xB);
	int & value2 = tree.getData(handler2);
	
	//check value and addr
	EXPECT_EQ(value1,value2);
	EXPECT_EQ(&value1,&value2);
}

/*******************  FUNCTION  *********************/
TEST(TestStackTreeSubSet,toJson)
{
	//build a short tree
	StackTreeSubSet<int> tree;
	StackTreeSubSet<int>::Handler handler = tree.buildNewhandler();
	tree.enter(handler,(void*)0xA);
	tree.enter(handler,(void*)0xB);
	
	//setup value
	int & value1 = tree.getData(handler);
	value1 = 110;
	
	//check json conversion
	std::stringstream out;
	htopml::convertToJson(out,tree);
	EXPECT_EQ(CST_VALUE_1,out.str());
}
