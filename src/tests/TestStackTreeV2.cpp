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

/*******************  FUNCTION  *********************/
TEST(TestTreeStackTracer,getNode)
{
	TreeStackTracer<int> tree;
	TreeStackTracer<int>::Node root = tree.getRootNode();
	TreeStackTracer<int>::Node node1 = tree.getNode(root,(void*)0xA);
	TreeStackTracer<int>::Node node2 = tree.getNode(node1,(void*)0xB);
	int & value1 = tree.getData(node2);
	value1 = 110;
	TreeStackTracer<int>::Node node3 = tree.getNode(node1,(void*)0xB);
	int & value2 = tree.getData(node3);
	EXPECT_EQ(value1,value2);
	EXPECT_EQ(&value1,&value2);
}

/*******************  FUNCTION  *********************/
TEST(TestTreeStackTracer,toJson)
{
	TreeStackTracer<int> tree;
	TreeStackTracer<int>::Node root = tree.getRootNode();
	TreeStackTracer<int>::Node node1 = tree.getNode(root,(void*)0xA);
	TreeStackTracer<int>::Node node2 = tree.getNode(node1,(void*)0xB);
	int & value1 = tree.getData(node2);
	value1 = 110;
	
	std::stringstream out;
	htopml::convertToJson(out,tree);
	EXPECT_EQ("",out.str());
}
