/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 10/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/stack-tree/tests/TestStackSTLHashMap.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (ECR) - 2014
*    AUTHOR   : Sébastien Valat - 2015
*    AUTHOR   : Bastien Levasseur - 2024
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
#include <gtest/gtest.h>
#include <json/ConvertToJson.h>
#include <common/SimpleAllocator.hpp>
#include <stack-tree/StackSTLHashMap.hpp>

/**********************************************************/
using namespace MALT;

void * CST_VALUE_1[] = {(void*)0x1,(void*)0x2,(void*)0x3,(void*)0x4};
const char CST_VALUE_2[] = "{\n\
	\"stats\":[{\n\
		\"stack\":[\"0x1\", \"0x2\", \"0x3\", \"0x4\"],\n\
		\"stackId\":\"0x1\",\n\
		\"infos\":11\n\
	}, {\n\
		\"stack\":[\"0x1\", \"0x2\", \"0x3\"],\n\
		\"stackId\":\"0x2\",\n\
		\"infos\":10\n\
	}],\n\
	\"count\":2\n\
}";


/**********************************************************/
TEST(StackSTLHashMap,constructor)
{
	StackSTLHashMap<int> map;
}

/**********************************************************/
TEST(StackSTLHashMap,getValueRef)
{
	StackSTLHashMap<int> map;
	Stack stack(CST_VALUE_1,4,STACK_ORDER_ASC, DOMAIN_C);
	
	map.getValueRef(stack) = 10;
	EXPECT_EQ(10,map.getValueRef(stack));
}

/**********************************************************/
TEST(StackSTLHashMap,operatorArray)
{
	StackSTLHashMap<int> map;
	Stack stack(CST_VALUE_1,4,STACK_ORDER_ASC, DOMAIN_C);
	
	map[stack] = 10;
	EXPECT_EQ(10,map[stack]);
}

/**********************************************************/
TEST(StackSTLHashMap,toJson)
{
	StackSTLHashMap<int> map;
	Stack stack1(CST_VALUE_1,3,STACK_ORDER_ASC, DOMAIN_C);
	map[stack1] = 10;
	Stack stack2(CST_VALUE_1,4,STACK_ORDER_ASC, DOMAIN_C);
	map[stack2] = 11;
	
	std::stringstream out;
	htopml::convertToJson(out,map);
	
	EXPECT_EQ(CST_VALUE_2,out.str());
}

/**********************************************************/
TEST(StackSTLHashMap,iterator)
{
	//fill
	StackSTLHashMap<int> map;
	Stack stack1(CST_VALUE_1,3,STACK_ORDER_ASC, DOMAIN_C);
	map[stack1] = 10;
	Stack stack2(CST_VALUE_1,4,STACK_ORDER_ASC, DOMAIN_C);
	map[stack2] = 11;
	Stack stack3(CST_VALUE_1,2,STACK_ORDER_ASC, DOMAIN_C);
	
	StackSTLHashMap<int>& cmap = map;
	StackSTLHashMap<int>::iterator it = cmap.find(stack1);
	EXPECT_EQ(stack1,*it->first.stack);
	EXPECT_EQ(10,it->second);
	it = cmap.find(stack2);
	EXPECT_EQ(stack2,*it->first.stack);
	EXPECT_EQ(11,it->second);
	it = cmap.find(stack3);
	EXPECT_EQ(cmap.end(),it);
}

/**********************************************************/
TEST(StackSTLHashMap,const_iterator)
{
	//fill
	StackSTLHashMap<int> map;
	Stack stack1(CST_VALUE_1,3,STACK_ORDER_ASC, DOMAIN_C);
	map[stack1] = 10;
	Stack stack2(CST_VALUE_1,4,STACK_ORDER_ASC, DOMAIN_C);
	map[stack2] = 11;
	Stack stack3(CST_VALUE_1,2,STACK_ORDER_ASC, DOMAIN_C);
	
	StackSTLHashMap<int>& cmap = map;
	StackSTLHashMap<int>::const_iterator it = cmap.find(stack1);
	EXPECT_EQ(stack1,*it->first.stack);
	EXPECT_EQ(10,it->second);
	it = cmap.find(stack2);
	EXPECT_EQ(stack2,*it->first.stack);
	EXPECT_EQ(11,it->second);
	it = cmap.find(stack3);
	EXPECT_EQ(cmap.end(),it);
}

/**********************************************************/
int main(int argc, char ** argv)
{
	//init internal allocator
	gblInternaAlloc = new SimpleAllocator(true);
	
	//help unit test
	gblUseFakeIdInOutput = 1;
	
	// This allows the user to override the flag on the command line.
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
