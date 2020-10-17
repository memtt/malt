/*****************************************************
             PROJECT  : MALT
             VERSION  : 1.2.2
             DATE     : 10/2020
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
#include <gtest/gtest.h>
#include <core/SimpleStackTracker.hpp>

/***************** USING NAMESPACE ******************/
using namespace MALT;

/********************** CONSTS **********************/
void * CST_STACK_1[] = {(void*)0x1,(void*)0x2,(void*)0x3,(void*)0x4,(void*)0x5,(void*)0x6,(void*)0x7,(void*)0x8,
                        (void*)0x9,(void*)0xa,(void*)0xb,(void*)0xc,(void*)0xd,(void*)0xe,(void*)0xf,(void*)0x4,
                        (void*)0x1,(void*)0x2,(void*)0x3,(void*)0x4,(void*)0x1,(void*)0x2,(void*)0x3,(void*)0x4,
                        (void*)0x1,(void*)0x2,(void*)0x3,(void*)0x4,(void*)0x1,(void*)0x2,(void*)0x3,(void*)0x4};
int CST_STACK_1_SIZE = 32;

/*******************  FUNCTION  *********************/
TEST(SimpleStackTracer,constructor)
{
	SimpleStackTracker tracker;
}

/*******************  FUNCTION  *********************/
TEST(SimpleStackTracer,findStackNodeAsc)
{
	SimpleStackTracker tracker;
	Stack stack(CST_STACK_1,CST_STACK_1_SIZE,STACK_ORDER_ASC);
	
	SimpleCallStackNode & node1 = tracker.getBacktraceInfo(stack);
	SimpleCallStackNode & node2 = tracker.getBacktraceInfo(stack);
	
	EXPECT_EQ(&node1,&node2);
}

/*******************  FUNCTION  *********************/
TEST(SimpleStackTracer,findStackNodeSkipDepthAsc)
{
	SimpleStackTracker tracker;
	Stack stack(CST_STACK_1,CST_STACK_1_SIZE,STACK_ORDER_ASC);
	
	SimpleCallStackNode & node1 = tracker.getBacktraceInfo(stack,5);
	SimpleCallStackNode & node2 = tracker.getBacktraceInfo(stack,5);
	
	EXPECT_EQ(&node1,&node2);
}

/*******************  FUNCTION  *********************/
TEST(SimpleStackTracer,findStackNodeDesc)
{
	SimpleStackTracker tracker;
	Stack stack(CST_STACK_1,CST_STACK_1_SIZE,STACK_ORDER_DESC);
	
	SimpleCallStackNode & node1 = tracker.getBacktraceInfo(stack);
	SimpleCallStackNode & node2 = tracker.getBacktraceInfo(stack);
	
	EXPECT_EQ(&node1,&node2);
}

/*******************  FUNCTION  *********************/
TEST(SimpleStackTracer,findStackNodeSkipDepthDesc)
{
	SimpleStackTracker tracker;
	Stack stack(CST_STACK_1,CST_STACK_1_SIZE,STACK_ORDER_DESC);
	
	SimpleCallStackNode & node1 = tracker.getBacktraceInfo(stack,5);
	SimpleCallStackNode & node2 = tracker.getBacktraceInfo(stack,5);
	
	EXPECT_EQ(&node1,&node2);
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
