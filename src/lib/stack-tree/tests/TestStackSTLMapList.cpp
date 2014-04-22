/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
#include <gtest/gtest.h>
#include <json/ConvertToJson.h>
#include <common/SimpleAllocator.hpp>
#include <stack-tree/StackSTLMapList.hpp>

/***************** USING NAMESPACE ******************/
using namespace MATT;

void * CST_VALUE_1[] = {(void*)0x1,(void*)0x2,(void*)0x3,(void*)0x4};

/*******************  FUNCTION  *********************/
TEST(StackSTLMapList,constructor)
{
	StackSTLMapList<int> map;
}

/*******************  FUNCTION  *********************/
TEST(StackSTLMapList,getValueRef)
{
	StackSTLMapList<int> map;
	Stack stack(CST_VALUE_1,4,STACK_ORDER_ASC);
	
	map.getValueRef(stack) = 10;
	EXPECT_EQ(10,map.getValueRef(stack));
}

/*******************  FUNCTION  *********************/
TEST(StackSTLMapList,operatorArray)
{
	StackSTLMapList<int> map;
	Stack stack(CST_VALUE_1,4,STACK_ORDER_ASC);
	
	map[stack] = 10;
	EXPECT_EQ(10,map[stack]);
}

/*******************  FUNCTION  *********************/
TEST(StackSTLMapList,toJson)
{
	StackSTLMapList<int> map;
	Stack stack1(CST_VALUE_1,3,STACK_ORDER_ASC);
	map[stack1] = 10;
	Stack stack2(CST_VALUE_1,4,STACK_ORDER_ASC);
	map[stack2] = 11;
	
	std::stringstream out;
	htopml::convertToJson(out,map);
	
	EXPECT_EQ("",out.str());
}

/*******************  FUNCTION  *********************/
int main(int argc, char ** argv)
{
	//init internal allocator
	gblInternaAlloc = new SimpleAllocator(true);
	
	// This allows the user to override the flag on the command line.
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
