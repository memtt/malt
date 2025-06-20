/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.3.1
*    DATE     : 06/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/stacks/tests/TestStackReducer.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
#include <gtest/gtest.h>
#include <stacks/Stack.hpp>
#include <common/SimpleAllocator.hpp>
#include <json/ConvertToJson.h>
#include <StackReducer.hpp>

/**********************************************************/
using namespace MALT;

/**********************************************************/
void * CST_STACK_1[] = {(void*)0x1,(void*)0x2,(void*)0x3,(void*)0x4,(void*)0x5,(void*)0x6,(void*)0x7};

/**********************************************************/
TEST(StackReducer,constructor)
{
	StackReducer reducer(3);
}

/**********************************************************/
TEST(StackReducer,reduce_1)
{
	EnterExitStack stack;
	for (size_t i = 1 ; i <= 7 ; i++)
		stack.enterFunction(LangAddress(DOMAIN_C, (void*)i));
	
	StackReducer reducer(3);
	//Stack stack(CST_STACK_1,7,STACK_ORDER_ASC, DOMAIN_C);
	std::cout << stack << std::endl;
	reducer.reduce(stack);
	std::cout << stack << std::endl;
	std::cout << reducer << std::endl;
	EnterExitStack ref;
	ref.enterFunction(LangAddress(DOMAIN_SUB_STACK, (void*)0x2));
	ref.enterFunction(LangAddress(DOMAIN_C, (void*)0x7));
	EXPECT_EQ(ref, stack);
}

/**********************************************************/
TEST(StackReducer,reduce_2)
{
	StackReducer reducer(3);
	Stack stack(CST_STACK_1,7,STACK_ORDER_DESC, DOMAIN_C);
	std::cout << stack << std::endl;
	reducer.reduce(stack);
	std::cout << stack << std::endl;
	std::cout << reducer << std::endl;
	EnterExitStack ref;
	ref.enterFunction(LangAddress(DOMAIN_SUB_STACK, (void*)0x2));
	ref.enterFunction(LangAddress(DOMAIN_C, (void*)0x7));
	EXPECT_EQ(ref, stack);
}

/**********************************************************/
TEST(StackReducer,reduce_3)
{
	StackReducer reducer(3);
	Stack stack(CST_STACK_1,7,STACK_ORDER_ASC, DOMAIN_C);
	std::cout << stack << std::endl;
	reducer.reduce(stack);
	std::cout << stack << std::endl;
	std::cout << reducer << std::endl;
	EnterExitStack ref;
	ref.enterFunction(LangAddress(DOMAIN_SUB_STACK, (void*)0x2));
	ref.enterFunction(LangAddress(DOMAIN_C, (void*)0x1));
	EXPECT_EQ(ref, stack);
}

/**********************************************************/
int main(int argc, char ** argv)
{
	//init internal allocator
	gblInternaAlloc = new SimpleAllocator(true);
	
	// This allows the user to override the flag on the command line.
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
