/*****************************************************
             PROJECT  : MALT
             VERSION  : 1.2.2
             DATE     : 10/2020
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
#include <gtest/gtest.h>
#include <core/SegmentTracker.hpp>
#include <stacks/Stack.hpp>

/***************** USING NAMESPACE ******************/
using namespace MALT;

/********************* GLOBALS **********************/
//for pre init of global allocator
class ForceInit
{
	public:
		ForceInit(void) {gblInternaAlloc = new SimpleAllocator(true);};
};
ForceInit gblForceInit;

/********************* GLOBALS **********************/
static void * CST_STACK_1_CONTENT[] = {(void*)0x1,(void*)0x2,(void*)0x3};
#define CST_STACK_1_SIZE (sizeof(CST_STACK_1_CONTENT)/sizeof(CST_STACK_1_CONTENT[0]))
static void * CST_ADDR1 = ((void*)0x42);
static Stack CST_STACK_1(CST_STACK_1_CONTENT,CST_STACK_1_SIZE,STACK_ORDER_ASC);
static CallStackInfo CST_INFO_1;

/********************  MACROS  **********************/
#define EXPECT_NULL(x) EXPECT_EQ((void*)NULL,(x))
#define EXPECT_NOT_NULL(x) EXPECT_NE((void*)NULL,(x))
#define ASSERT_NULL(x) ASSERT_EQ((void*)NULL,(x))
#define ASSERT_NOT_NULL(x) ASSERT_NE((void*)NULL,(x))

/*******************  FUNCTION  *********************/
TEST(TestSegmentTracker,testConstructor)
{
	SegmentTracker tracker;
}

/*******************  FUNCTION  *********************/
TEST(TestSegmentTracker,testAdd)
{
	//setup objects
	SegmentTracker tracker;
	MMCallStackNode stack(&CST_STACK_1,&CST_INFO_1);

	//try to add a new entry
	tracker.add(CST_ADDR1,64,stack);
}

/*******************  FUNCTION  *********************/
TEST(TestSegmentTracker,testGetOK)
{
	//setup objects
	SegmentTracker tracker;
	MMCallStackNode stack(&CST_STACK_1,&CST_INFO_1);
	
	//try to add a new entry
	tracker.add(CST_ADDR1,64,stack);
	
	//try to get
	SegmentInfo * info = tracker.get(CST_ADDR1);
	ASSERT_NOT_NULL(info);
	EXPECT_EQ(64UL,info->size);
}

/*******************  FUNCTION  *********************/
TEST(TestSegmentTracker,testGetInvalid)
{
	//setup objects
	SegmentTracker tracker;
	MMCallStackNode stack(&CST_STACK_1,&CST_INFO_1);
	
	//try to add a new entry
	tracker.add(CST_ADDR1,64,stack);
	
	//try to get
	EXPECT_NULL(tracker.get((void*)0x41));
}

/*******************  FUNCTION  *********************/
TEST(TestSegmentTracker,testRemoveOK)
{
	//setup objects
	SegmentTracker tracker;
	MMCallStackNode stack(&CST_STACK_1,&CST_INFO_1);

	//try to add a new entry
	tracker.add(CST_ADDR1,64,stack);
	
	EXPECT_NOT_NULL(tracker.get(CST_ADDR1));
	tracker.remove(CST_ADDR1);
	EXPECT_NULL(tracker.get(CST_ADDR1));
}

/*******************  FUNCTION  *********************/
TEST(TestSegmentTracker,testRemoveNotExist)
{
	//setup objects
	SegmentTracker tracker;
	SimpleCallStackNode stack(CST_STACK_1);
	
	EXPECT_NULL(tracker.get(CST_ADDR1));
	tracker.remove(CST_ADDR1);
	EXPECT_NULL(tracker.get(CST_ADDR1));
}

/*******************  FUNCTION  *********************/
TEST(TestSegmentTracker,testMunmap1)
{
	//setup objects
	SegmentTracker tracker;
	MMCallStackNode stack(&CST_STACK_1,&CST_INFO_1);
	
	//try to add a new entry
	tracker.add((void*)0x100,0x100,stack);
	
	//munmap out of segment (after)
	tracker.munmap((void*)0x300,0x100);
	
	SegmentInfo * info = tracker.get((void*)0x100);
	EXPECT_NOT_NULL(info);
	EXPECT_EQ(0x100U,info->size);
}

/*******************  FUNCTION  *********************/
TEST(TestSegmentTracker,testMunmap2)
{
	//setup objects
	SegmentTracker tracker;
	MMCallStackNode stack(&CST_STACK_1,&CST_INFO_1);
	
	//try to add a new entry
	tracker.add((void*)0x100,0x100,stack);
	
	//munmap out of segment (left)
	tracker.munmap((void*)0x001,0x10);
	
	SegmentInfo * info = tracker.get((void*)0x100);
	EXPECT_NOT_NULL(info);
	EXPECT_EQ(0x100U,info->size);
}

/*******************  FUNCTION  *********************/
TEST(TestSegmentTracker,testMunmap3)
{
	//setup objects
	SegmentTracker tracker;
	MMCallStackNode stack(&CST_STACK_1,&CST_INFO_1);
	
	//try to add a new entry
	tracker.add((void*)0x100,0x100,stack);
	
	//munmap out of segment (after)
	tracker.munmap((void*)0x100,0x080);
	
	SegmentInfo * info = tracker.get((void*)0x100);
	EXPECT_NULL(info);
	
	info = tracker.get((void*)0x180);
	ASSERT_NOT_NULL(info);
	EXPECT_EQ(0x080U,info->size);
}

/*******************  FUNCTION  *********************/
TEST(TestSegmentTracker,testMunmap4)
{
	//setup objects
	SegmentTracker tracker;
	MMCallStackNode stack(&CST_STACK_1,&CST_INFO_1);
	
	//try to add a new entry
	tracker.add((void*)0x100,0x100,stack);
	
	//munmap out of segment (after)
	tracker.munmap((void*)0x180,0x080);
	
	SegmentInfo * info = tracker.get((void*)0x180);
	EXPECT_NULL(info);
	
	info = tracker.get((void*)0x100);
	ASSERT_NOT_NULL(info);
	EXPECT_EQ(0x080U,info->size);
}

/*******************  FUNCTION  *********************/
TEST(TestSegmentTracker,testMunmap5)
{
	//setup objects
	SegmentTracker tracker;
	MMCallStackNode stack(&CST_STACK_1,&CST_INFO_1);
	
	//try to add a new entry
	tracker.add((void*)0x100,0x100,stack);
	
	//munmap out of segment (after)
	tracker.munmap((void*)0x140,0x080);
	
	SegmentInfo * info = tracker.get((void*)0x100);
	ASSERT_NOT_NULL(info);
	EXPECT_EQ(0x040U,info->size);
	
	info = tracker.get((void*)0x1C0);
	ASSERT_NOT_NULL(info);
	EXPECT_EQ(0x040U,info->size);
}
