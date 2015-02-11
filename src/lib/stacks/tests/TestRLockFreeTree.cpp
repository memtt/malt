/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 02/2015
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
#include <gtest/gtest.h>
#include <json/ConvertToJson.h>
#include <allocators/SimpleAllocator.hpp>
#include <stacks/RLockFreeTree.hpp>

/***************** USING NAMESPACE ******************/
using namespace MATT;

void * CST_VALUE_1[] = {(void*)0x1,(void*)0x2,(void*)0x3,(void*)0x4};
const char CST_VALUE_2[] =  "{\n\
\t\"calltree\":{\n\
\t\t\"0xbbb\":{\n\
\t\t\t\"0xccc\":{\n\
\t\t\t\t\"dataId\":2\n\
\t\t\t}\n\
\t\t},\n\
\t\t\"0xaaa\":{\n\
\t\t\t\"dataId\":0\n\
\t\t}\n\
\t},\n\
\t\"data\":{\n\
\t\t\"test-counter\":{\n\
\t\t\t\"2\":11,\n\
\t\t\t\"0\":10\n\
\t\t}\n\
\t}\n}";

/*******************  FUNCTION  *********************/
TEST(TypedRLockFreeTree,constructor)
{
	RLockFreeTree tree;
}

/*******************  FUNCTION  *********************/
TEST(TypedRLockFreeTree,enterThread)
{
	RLockFreeTree tree;
	tree.addDescriptor<int>("test-counter");
	
	StackTreeHandler handler1 = tree.enterThread();
	StackTreeHandler handler2 = tree.enterThread();
	
	EXPECT_EQ(handler1,handler2);
	EXPECT_NE((void*)NULL,handler1);
}

/*******************  FUNCTION  *********************/
TEST(TypedRLockFreeTree,enterFunction)
{
	RLockFreeTree tree;
	tree.addDescriptor<int>("test-counter");

	StackTreeHandler handler1 = tree.enterThread();
	StackTreeHandler handler2 = tree.enterFunction(handler1,(void*)0xAAA);
	StackTreeHandler handler3 = tree.enterFunction(handler1,(void*)0xAAA);
	StackTreeHandler handler4 = tree.enterFunction(handler1,(void*)0xBBB);
	
	EXPECT_EQ(handler2,handler3);
	EXPECT_NE(handler1,handler2);
	EXPECT_NE(handler1,handler4);
}

/*******************  FUNCTION  *********************/
TEST(TypedRLockFreeTree,getData)
{
	RLockFreeTree tree;
	tree.addDescriptor<int>("test-counter");

	StackTreeHandler handler1 = tree.enterThread();
	StackTreeHandler handler2 = tree.enterFunction(handler1,(void*)0xAAA);
	StackTreeHandler handler3 = tree.enterFunction(handler1,(void*)0xBBB);
	
	int * data21 = &tree.getTypedData<int>(handler2,0);
	int * data22 = &tree.getTypedData<int>(handler2,0);
	int * data31 = &tree.getTypedData<int>(handler3,0);
	int * data32 = &tree.getTypedData<int>(handler3,0);
	
	EXPECT_EQ(data21,data22);
	EXPECT_EQ(data31,data32);
	EXPECT_NE(data21,data31);
}

/*******************  FUNCTION  *********************/
TEST(TypedRLockFreeTree,toJson)
{
	RLockFreeTree tree;
	tree.addDescriptor<int>("test-counter");

	StackTreeHandler handler1 = tree.enterThread();
	StackTreeHandler handler2 = tree.enterFunction(handler1,(void*)0xAAA);
	StackTreeHandler handler3 = tree.enterFunction(handler1,(void*)0xBBB);
	StackTreeHandler handler4 = tree.enterFunction(handler3,(void*)0xCCC);
	
	tree.getTypedData<int>(handler2,0) = 10;
	tree.getTypedData<int>(handler4,0) = 11;
	
	std::stringstream out;
	tree.prepareForOutput();
	htopml::convertToJson(out,tree);
	
	EXPECT_EQ(CST_VALUE_2,out.str());
}

/*******************  FUNCTION  *********************/
TEST(TypedRLockFreeTree,parallelUse)
{
	RLockFreeTree tree;
	tree.addDescriptor<int>("test-counter");

	#pragma omp parallel
	{
		for (int j = 0 ; j < 1000 ; j++)
		{
			StackTreeHandler handler = tree.enterThread();
			
			for (int i = 0 ; i < 10 ; i++)
				handler = tree.enterFunction(handler,(void*)(size_t)(i+1));
		}
	}
}

