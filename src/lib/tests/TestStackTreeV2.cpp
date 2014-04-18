/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
#include <gtest/gtest.h>
#include <stackv2/TreeStackTracer.hpp>

/***************** USING NAMESPACE ******************/
using namespace MATT;

/********************** CONSTS **********************/
static const char CST_VALUE_1[] = "[{\n\t\"stack\":[\"0xb\", \"0xa\"],\n\t\"infos\":110\n}]";

/*******************  FUNCTION  *********************/
TEST(TestTreeStackTracer,getNode)
{
	//build a short tree
	TreeStackTracer<int> tree;
	TreeStackTracer<int>::Node root = tree.getRootNode();
	TreeStackTracer<int>::Node node1 = tree.getNode(root,(void*)0xA);
	TreeStackTracer<int>::Node node2 = tree.getNode(node1,(void*)0xB);
	
	//try to get values
	int & value1 = tree.getData(node2);
	value1 = 110;
	
	//check if we reuse the value on next call
	TreeStackTracer<int>::Node node3 = tree.getNode(node1,(void*)0xB);
	int & value2 = tree.getData(node3);
	EXPECT_EQ(value1,value2);
	EXPECT_EQ(&value1,&value2);
}

/*******************  FUNCTION  *********************/
TEST(TestTreeStackTracer,toJson)
{
	//build a short tree
	TreeStackTracer<int> tree;
	TreeStackTracer<int>::Node root = tree.getRootNode();
	TreeStackTracer<int>::Node node1 = tree.getNode(root,(void*)0xA);
	TreeStackTracer<int>::Node node2 = tree.getNode(node1,(void*)0xB);
	
	//setup value
	int & value1 = tree.getData(node2);
	value1 = 110;
	
	//check json conversion
	std::stringstream out;
	htopml::convertToJson(out,tree);
	EXPECT_EQ(CST_VALUE_1,out.str());
}
