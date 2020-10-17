/*****************************************************
             PROJECT  : MALT
             VERSION  : 1.2.2
             DATE     : 10/2020
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
#include <gtest/gtest.h>
#include <json/ConvertToJson.h>
#include <common/SimpleAllocator.hpp>
#include <stack-tree/StackSTLHashMap.hpp>

/***************** USING NAMESPACE ******************/
using namespace MALT;

void * CST_VALUE_1[] = {(void*)0x1,(void*)0x2,(void*)0x3,(void*)0x4};
const char CST_VALUE_2[] = "{\n\
	\"stats\":[{\n\
		\"stack\":[\"0x1\", \"0x2\", \"0x3\"],\n\
		\"stackId\":\"0x1\",\n\
		\"infos\":10\n\
	}, {\n\
		\"stack\":[\"0x1\", \"0x2\", \"0x3\", \"0x4\"],\n\
		\"stackId\":\"0x2\",\n\
		\"infos\":11\n\
	}],\n\
	\"count\":2\n\
}";


/*******************  FUNCTION  *********************/
TEST(StackSTLHashMap,constructor)
{
	StackSTLHashMap<int> map;
}

/*******************  FUNCTION  *********************/
TEST(StackSTLHashMap,getValueRef)
{
	StackSTLHashMap<int> map;
	Stack stack(CST_VALUE_1,4,STACK_ORDER_ASC);
	
	map.getValueRef(stack) = 10;
	EXPECT_EQ(10,map.getValueRef(stack));
}

/*******************  FUNCTION  *********************/
TEST(StackSTLHashMap,operatorArray)
{
	StackSTLHashMap<int> map;
	Stack stack(CST_VALUE_1,4,STACK_ORDER_ASC);
	
	map[stack] = 10;
	EXPECT_EQ(10,map[stack]);
}

/*******************  FUNCTION  *********************/
TEST(StackSTLHashMap,toJson)
{
	StackSTLHashMap<int> map;
	Stack stack1(CST_VALUE_1,3,STACK_ORDER_ASC);
	map[stack1] = 10;
	Stack stack2(CST_VALUE_1,4,STACK_ORDER_ASC);
	map[stack2] = 11;
	
	std::stringstream out;
	htopml::convertToJson(out,map);
	
	EXPECT_EQ(CST_VALUE_2,out.str());
}

/*******************  FUNCTION  *********************/
TEST(StackSTLHashMap,iterator)
{
	//fill
	StackSTLHashMap<int> map;
	Stack stack1(CST_VALUE_1,3,STACK_ORDER_ASC);
	map[stack1] = 10;
	Stack stack2(CST_VALUE_1,4,STACK_ORDER_ASC);
	map[stack2] = 11;
	
	StackSTLHashMap<int>::iterator it = map.begin();
	EXPECT_EQ(stack1,*it->first.stack);
	EXPECT_EQ(10,it->second);
	++it;
	EXPECT_EQ(stack2,*it->first.stack);
	EXPECT_EQ(11,it->second);
	++it;
	EXPECT_EQ(map.end(),it);
}

/*******************  FUNCTION  *********************/
TEST(StackSTLHashMap,const_iterator)
{
	//fill
	StackSTLHashMap<int> map;
	Stack stack1(CST_VALUE_1,3,STACK_ORDER_ASC);
	map[stack1] = 10;
	Stack stack2(CST_VALUE_1,4,STACK_ORDER_ASC);
	map[stack2] = 11;
	
	StackSTLHashMap<int>& cmap = map;
	StackSTLHashMap<int>::const_iterator it = cmap.begin();
	EXPECT_EQ(stack1,*it->first.stack);
	EXPECT_EQ(10,it->second);
	++it;
	EXPECT_EQ(stack2,*it->first.stack);
	EXPECT_EQ(11,it->second);
	++it;
	EXPECT_EQ(cmap.end(),it);
}

/*******************  FUNCTION  *********************/
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
