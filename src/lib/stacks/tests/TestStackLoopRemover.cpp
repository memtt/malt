/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 02/2015
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
#include <gtest/gtest.h>
#include <stacks/Stack.hpp>
#include <StackLoopRemover.hpp>
#include <allocators/SimpleAllocator.hpp>
#include <json/ConvertToJson.h>

/***************** USING NAMESPACE ******************/
using namespace MATT;

/********************** CONSTS **********************/
void * CST_STACK_1[] = {(void*)0x1,(void*)0x2,(void*)0x3,(void*)0x4,(void*)0x5};
void * CST_STACK_2[] = {(void*)0x1,(void*)0x2,(void*)0x2,(void*)0x2,(void*)0x3};
void * CST_STACK_3[] = {(void*)0x1,(void*)0x2,(void*)0x2,(void*)0x3};

/*******************  FUNCTION  *********************/
TEST(Stack,noSuppress)
{
	Stack stack(STACK_ORDER_ASC);
	EXPECT_EQ(0,stack.getSize());
	stack.set(CST_STACK_1,5,STACK_ORDER_ASC);
	StackLoopRemover remover(5);
	remover.removeLoops(stack);
	EXPECT_EQ(5,stack.getSize());
	
	for (int i = 0 ; i < 5 ; i++)
		EXPECT_EQ(stack[i],CST_STACK_1[i]);
	
}

/*******************  FUNCTION  *********************/
TEST(Stack,simpleCase1)
{
	Stack stack(STACK_ORDER_ASC);
	EXPECT_EQ(0,stack.getSize());
	stack.set(CST_STACK_2,5,STACK_ORDER_ASC);
	StackLoopRemover remover(5);
	remover.removeLoops(stack);
	EXPECT_EQ(4,stack.getSize());
	
	for (int i = 0 ; i < 4 ; i++)
		EXPECT_EQ(stack[i],CST_STACK_3[i]);
	
}
