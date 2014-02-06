/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
#include <gtest/gtest.h>
#include <core/Stack.h>
#include <json/ConvertToJson.h>

/***************** USING NAMESPACE ******************/
using namespace MATT;

/********************** CONSTS **********************/
void * CST_STACK_1[] = {(void*)0x1,(void*)0x2,(void*)0x3,(void*)0x4};

/*******************  FUNCTION  *********************/
TEST(Stack,constructorDefault)
{
	Stack stack(STACK_ORDER_ASC);
	EXPECT_EQ(0,stack.getSize());
}

/*******************  FUNCTION  *********************/
TEST(Stack,constructorArg)
{
	Stack stack(CST_STACK_1,4,STACK_ORDER_ASC);
	EXPECT_EQ(4,stack.getSize());
}

/*******************  FUNCTION  *********************/
TEST(Stack,isValid)
{
	Stack stack(STACK_ORDER_ASC);
	EXPECT_FALSE(stack.isValid());
	stack.set(CST_STACK_1,4,STACK_ORDER_ASC);
	EXPECT_EQ(4,stack.getSize());
	EXPECT_TRUE(stack.isValid());
}

/*******************  FUNCTION  *********************/
TEST(Stack,getCalleeAsc)
{
	Stack stack(CST_STACK_1,4,STACK_ORDER_ASC);
	EXPECT_EQ((void*)0x1,stack.getCallee());
}

/*******************  FUNCTION  *********************/
TEST(Stack,getCalleeDesc)
{
	Stack stack(CST_STACK_1,4,STACK_ORDER_DESC);
	EXPECT_EQ((void*)0x4,stack.getCallee());
}

/*******************  FUNCTION  *********************/
TEST(Stack,getCallerAsc)
{
	Stack stack(CST_STACK_1,4,STACK_ORDER_ASC);
	EXPECT_EQ((void*)0x2,stack.getCaller());
}

/*******************  FUNCTION  *********************/
TEST(Stack,getCallerDesc)
{
	Stack stack(CST_STACK_1,4,STACK_ORDER_DESC);
	EXPECT_EQ((void*)0x3,stack.getCaller());
}

/*******************  FUNCTION  *********************/
TEST(Stack,operatorArrayAsc)
{
	Stack stack(CST_STACK_1,4,STACK_ORDER_ASC);
	for (size_t i = 0 ; i < 4 ; i++)
		EXPECT_EQ((void*)(i+1),stack[i]);
}

/*******************  FUNCTION  *********************/
TEST(Stack,operatorArrayDesc)
{
	Stack stack(CST_STACK_1,4,STACK_ORDER_DESC);
	for (size_t i = 0 ; i < 4 ; i++)
		EXPECT_EQ((void*)(4-i),stack[i]);
}

/*******************  FUNCTION  *********************/
TEST(Stack,partialCompareDesc1)
{
	Stack stack1(CST_STACK_1,4,STACK_ORDER_DESC);
	Stack stack2(CST_STACK_1,4,STACK_ORDER_DESC);

	EXPECT_TRUE(Stack::partialCompare(stack1,0,stack2,0));
	EXPECT_TRUE(Stack::partialCompare(stack1,1,stack2,1));
	EXPECT_FALSE(Stack::partialCompare(stack1,0,stack2,1));
}

/*******************  FUNCTION  *********************/
TEST(Stack,partialCompareDesc2)
{
	Stack stack1(CST_STACK_1,3,STACK_ORDER_DESC);
	Stack stack2(CST_STACK_1,4,STACK_ORDER_DESC);
	Stack stack3(CST_STACK_1+1,3,STACK_ORDER_DESC);

	EXPECT_FALSE(Stack::partialCompare(stack1,0,stack2,0));
	EXPECT_FALSE(Stack::partialCompare(stack1,1,stack2,1));
	EXPECT_TRUE(Stack::partialCompare(stack1,0,stack2,1));
	EXPECT_FALSE(Stack::partialCompare(stack3,0,stack2,1));
}

/*******************  FUNCTION  *********************/
TEST(Stack,partialCompareAsc1)
{
	Stack stack1(CST_STACK_1,4,STACK_ORDER_ASC);
	Stack stack2(CST_STACK_1,4,STACK_ORDER_ASC);

	EXPECT_TRUE(Stack::partialCompare(stack1,0,stack2,0));
	EXPECT_TRUE(Stack::partialCompare(stack1,1,stack2,1));
	EXPECT_FALSE(Stack::partialCompare(stack1,0,stack2,1));
}

/*******************  FUNCTION  *********************/
TEST(Stack,partialCompareAsc2)
{
	Stack stack1(CST_STACK_1,3,STACK_ORDER_ASC);
	Stack stack2(CST_STACK_1,4,STACK_ORDER_ASC);
	Stack stack3(CST_STACK_1+1,3,STACK_ORDER_ASC);

	EXPECT_FALSE(Stack::partialCompare(stack1,0,stack2,0));
	EXPECT_FALSE(Stack::partialCompare(stack1,1,stack2,1));
	EXPECT_FALSE(Stack::partialCompare(stack1,0,stack2,1));
	EXPECT_TRUE(Stack::partialCompare(stack3,0,stack2,1));
}

/*******************  FUNCTION  *********************/
TEST(Stack,operatorStreamAsc)
{
	Stack stack(CST_STACK_1,4,STACK_ORDER_ASC);

	std::stringstream buffer;
	buffer << stack;
	
	EXPECT_EQ("0x1 0x2 0x3 0x4 ",buffer.str());
}

/*******************  FUNCTION  *********************/
TEST(Stack,operatorStreamDesc)
{
	Stack stack(CST_STACK_1,4,STACK_ORDER_DESC);

	std::stringstream buffer;
	buffer << stack;
	
	EXPECT_EQ("0x4 0x3 0x2 0x1 ",buffer.str());
}

/*******************  FUNCTION  *********************/
TEST(Stack,convertToJsonAsc)
{
	Stack stack(CST_STACK_1,4,STACK_ORDER_ASC);
	
	std::stringstream buffer;
	htopml::convertToJson(buffer,stack);
	
	EXPECT_EQ("[\"0x1\", \"0x2\", \"0x3\", \"0x4\"]",buffer.str());
}

/*******************  FUNCTION  *********************/
TEST(Stack,convertToJsonDesc)
{
	Stack stack(CST_STACK_1,4,STACK_ORDER_DESC);
	
	std::stringstream buffer;
	htopml::convertToJson(buffer,stack);
	
	EXPECT_EQ("[\"0x4\", \"0x3\", \"0x2\", \"0x1\"]",buffer.str());
}
