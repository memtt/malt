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
#include "../StackTreeMap.hpp"

/***************** USING NAMESPACE ******************/
using namespace MALTV2;

void * CST_VALUE_1[] = {(void*)0x1,(void*)0x2,(void*)0x3,(void*)0x4};
const char CST_VALUE_2[] = "{\n\t\"addresses\":{\n\t\t\"0x3\":\"0xaaa\",\n\t\t\"0x1\":\"0xbbb\",\n\t\t\"0x2\":\"0xccc\"\n\t},\n\t\"calltree\":{\n\t\t\"0x1\":{\n\t\t\t\"0x2\":{\n\t\t\t\t\"dataId\":2\n\t\t\t}\n\t\t},\n\t\t\"0x3\":{\n\t\t\t\"dataId\":0\n\t\t}\n\t},\n\t\"data\":{\n\t\t\"test-counter\":{\n\t\t\t\"2\":11,\n\t\t\t\"0\":10\n\t\t}\n\t}\n}"
;

/*******************  FUNCTION  *********************/
TEST(StackTreeMap,constructor)
{
	StackTreeMap map;
}

/*******************  FUNCTION  *********************/
TEST(StackTreeMap,getValueRef)
{
	//setup map
	StackTreeMap map;
	int id = map.addDescriptor<int>("test-counter");
	EXPECT_EQ(0,id);

	//setup stack
	Stack stack(CST_VALUE_1,4,STACK_ORDER_ASC);
	
	//move to entry
	StackTreeHandler handler = map.enterThread();
	handler = map.getFromStack(handler,stack);
	
	//try
	map.getTypedData<int>(handler,0) = 10;
	EXPECT_EQ(10,map.getTypedData<int>(handler,0));
}

/*******************  FUNCTION  *********************/
TEST(StackTreeMap,toJson)
{
	//setup map
	StackTreeMap map;
	int id = map.addDescriptor<int>("test-counter");
	EXPECT_EQ(0,id);
	
	//move to entry
	StackTreeHandler handler = map.enterThread();
	
	//setup
	static void * sstack1[] = {(void*)0xAAA,(void*)0xDDD};
	static void * sstack2[] = {(void*)0xBBB,(void*)0xCCC,(void*)0xDDD};
	
	//setup stack
	Stack stack1(sstack1,1,STACK_ORDER_ASC);
	Stack stack2(sstack2,1,STACK_ORDER_ASC);
	Stack stack3(sstack2,2,STACK_ORDER_ASC);
	
	//setup
	handler = map.getFromStack(handler,stack1);
	map.getTypedData<int>(handler,0) = 10;
	handler = map.getFromStack(handler,stack3);
	map.getTypedData<int>(handler,0) = 11;
	
	map.prepareForOutput();
	std::stringstream out;
	htopml::convertToJson(out,map);
	//try
	EXPECT_EQ(CST_VALUE_2,out.str());
}

/*******************  FUNCTION  *********************/
TEST(StackTreeMap,testDataHandler)
{
	//setup map
	StackTreeMap map;
	int id = map.addDescriptor<int>("test-counter");
	EXPECT_EQ(0,id);
	
	//move to entry
	StackTreeHandler handler = map.enterThread();
	
	//setup
	static void * sstack1[] = {(void*)0xAAA,(void*)0xDDD};
	
	//setup stack
	Stack stack1(sstack1,2,STACK_ORDER_ASC);
	handler = map.getFromStack(handler,stack1);
	
	//get data handler (eg. in malloc)
	StackTreeDataHandler dataHandler = map.getDataHandler(handler);
	
	//now try to setup valu (eg. in later in free)
	map.getTypedData<int>(dataHandler,0) = 10;
	
	EXPECT_EQ(10,map.getTypedData<int>(dataHandler,0));
}

/*******************  FUNCTION  *********************/
int main(int argc, char ** argv)
{
	//init internal allocator
	MALT::initInternalAlloc();
	doNoFreeAllocatorInit();
	
	// This allows the user to override the flag on the command line.
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

