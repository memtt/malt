/*****************************************************
             PROJECT  : MALT
             VERSION  : 1.2.2
             DATE     : 10/2020
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
#include <gtest/gtest.h>
#include <stacks/Stack.hpp>
#include <common/SimpleAllocator.hpp>
#include <json/ConvertToJson.h>

/***************** USING NAMESPACE ******************/
using namespace MALT;

/********************** CONSTS **********************/
void * CST_STACK_1[] = {(void*)0x1,(void*)0x2,(void*)0x3,(void*)0x4};
void * CST_STACK_2[] = {(void*)0x4,(void*)0x3,(void*)0x2,(void*)0x1};

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

/*******************  FUNCTION  *********************/
TEST(Stack,copyConstructor)
{
	Stack stack(CST_STACK_1,4,STACK_ORDER_DESC);
	Stack stack2(stack);
	
	EXPECT_EQ(stack,stack2);
}

/*******************  FUNCTION  *********************/
TEST(Stack,set)
{
	Stack stack(CST_STACK_1,4,STACK_ORDER_DESC);
	Stack stack2(STACK_ORDER_DESC);
	stack2.set(stack);
	
	EXPECT_EQ(stack,stack2);
}

/*******************  FUNCTION  *********************/
TEST(Stack,partialCopyConstructor1)
{
	Stack stack(CST_STACK_1,4,STACK_ORDER_DESC);
	Stack stack2(stack,2);
	
	EXPECT_EQ(2,stack2.getSize());
	EXPECT_EQ((void*)0x2,stack2[0]);
	EXPECT_EQ((void*)0x1,stack2[1]);
}

/*******************  FUNCTION  *********************/
TEST(Stack,partialCopyConstructor)
{
	Stack stack(CST_STACK_1,4,STACK_ORDER_ASC);
	Stack stack2(stack,2);
	
	EXPECT_EQ(2,stack2.getSize());
	EXPECT_EQ((void*)0x3,stack2[0]);
	EXPECT_EQ((void*)0x4,stack2[1]);
}

/*******************  FUNCTION  *********************/
TEST(Stack,hash)
{
	Stack stack1(CST_STACK_1,4,STACK_ORDER_ASC);
	Stack stack2(CST_STACK_2,4,STACK_ORDER_DESC);
	
	StackHash hash1 = stack1.hash();
	StackHash hash2 = stack2.hash();
	
	EXPECT_EQ(hash1,hash2);
}

/*******************  FUNCTION  *********************/
TEST(Stack,hashPartial)
{
	Stack stack1(CST_STACK_1,4,STACK_ORDER_ASC);
	Stack stack2(CST_STACK_2,4,STACK_ORDER_DESC);
	
	StackHash hash1 = stack1.hash(2);
	StackHash hash2 = stack2.hash(2);
	
	EXPECT_EQ(hash1,hash2);
}

/*******************  FUNCTION  *********************/
TEST(Stack,setReverse)
{
	Stack stack1(CST_STACK_1,4,STACK_ORDER_ASC);
	Stack stack2(STACK_ORDER_ASC);
	
	stack2.set(CST_STACK_2,4,STACK_ORDER_DESC);
	
	EXPECT_EQ(stack1,stack2);
}

/*******************  FUNCTION  *********************/
TEST(Stack,operatorEqual)
{
	Stack stack1(CST_STACK_1,4,STACK_ORDER_ASC);
	Stack stack2(STACK_ORDER_ASC);
	
	stack2 = stack1;

	EXPECT_EQ(stack1,stack2);
}

/*******************  FUNCTION  *********************/
TEST(Stack,grow)
{
	Stack stack(STACK_ORDER_ASC);
	size_t s1 = stack.getMemSize();
	stack.grow();
	size_t s2 = stack.getMemSize();
	stack.grow();
	size_t s3 = stack.getMemSize();
	stack.grow();
	size_t s4 = stack.getMemSize();
	
	EXPECT_GT(s2,s1);
	EXPECT_GT(s3,s2);
	EXPECT_GT(s4,s3);
}


/*******************  FUNCTION  *********************/
TEST(Stack,large)
{
	Stack stack1(CST_STACK_1,4,STACK_ORDER_ASC);
	
	void ** tmp = new void*[1024*1024];
	stack1.set(tmp,1024*1024,STACK_ORDER_ASC);
	
	EXPECT_EQ(1024*1024,stack1.getSize());
}

/*******************  FUNCTION  *********************/
TEST(Stack,fastSkip1)
{
	Stack stack(CST_STACK_1,4,STACK_ORDER_ASC);
	stack.fastSkip(1);

	std::stringstream buffer;
	buffer << stack;
	
	EXPECT_EQ("0x2 0x3 0x4 ",buffer.str());
}

/*******************  FUNCTION  *********************/
TEST(Stack,fastSkip2)
{
	Stack stack(CST_STACK_2,4,STACK_ORDER_DESC);
	stack.fastSkip(1);

	std::stringstream buffer;
	buffer << stack;
	
	EXPECT_EQ("0x2 0x3 0x4 ",buffer.str());
}

/*******************  FUNCTION  *********************/
int main(int argc, char ** argv)
{
	//init internal allocator
	initInternalAlloc();
	
	// This allows the user to override the flag on the command line.
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

