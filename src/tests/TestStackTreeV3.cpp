/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
#include <gtest/gtest.h>
#include <stackv3/TreeStackTracerV3.hpp>

/***************** USING NAMESPACE ******************/
using namespace MATT;

/********************** CONSTS **********************/
static const char CST_VALUE_1[] = "[{\n\t\"stack\":[\"0xb\", \"0xa\"],\n\t\"infos\":110\n}]";

/*******************  FUNCTION  *********************/
TEST(TestTreeStackTracerV3,getNode)
{
	//build a short tree
	TreeStackTracerV3<int> tree;
	TreeStackTracerV3<int>::Node root = tree.getRootNode();
	TreeStackTracerV3<int>::Node node1 = tree.getNode(root,(void*)0xA);
	TreeStackTracerV3<int>::Node node2 = tree.getNode(node1,(void*)0xB);
	
	//try to get values
	int & value1 = tree.getData(node2);
	value1 = 110;
	
	//check if we reuse the value on next call
	TreeStackTracerV3<int>::Node node3 = tree.getNode(node1,(void*)0xB);
	int & value2 = tree.getData(node3);
	EXPECT_EQ(value1,value2);
	EXPECT_EQ(&value1,&value2);
}

/*******************  FUNCTION  *********************/
TEST(TestTreeStackTracerV3,toJson)
{
	//build a short tree
	TreeStackTracerV3<int> tree;
	TreeStackTracerV3<int>::Node root = tree.getRootNode();
	TreeStackTracerV3<int>::Node node1 = tree.getNode(root,(void*)0xA);
	TreeStackTracerV3<int>::Node node2 = tree.getNode(node1,(void*)0xB);
	
	//setup value
	int & value1 = tree.getData(node2);
	value1 = 110;
	
	//check json conversion
	std::stringstream out;
	htopml::convertToJson(out,tree);
	EXPECT_EQ(CST_VALUE_1,out.str());
}
