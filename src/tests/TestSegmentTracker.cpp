/********************  HEADERS  *********************/
#include <gtest/gtest.h>
#include <lib/SegmentTracker.h>
#include <lib/Stack.h>

/********************* GLOBALS **********************/
static void * CST_STACK_1_CONTENT[] = {(void*)0x1,(void*)0x2,(void*)0x3};
#define CST_STACK_1_SIZE (sizeof(CST_STACK_1_CONTENT)/sizeof(CST_STACK_1_CONTENT[0]))
static void * CST_ADDR1 = ((void*)0x42);
static Stack CST_STACK_1(CST_STACK_1_CONTENT,CST_STACK_1_SIZE,STACK_ORDER_ASC);

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
	SimpleCallStackNode stack(CST_STACK_1);

	//try to add a new entry
	tracker.add(CST_ADDR1,64,&stack);
}

/*******************  FUNCTION  *********************/
TEST(TestSegmentTracker,testGetOK)
{
	//setup objects
	SegmentTracker tracker;
	SimpleCallStackNode stack(CST_STACK_1);
	
	//try to add a new entry
	tracker.add(CST_ADDR1,64,&stack);
	
	//try to get
	SegmentInfo * info = tracker.get(CST_ADDR1);
	ASSERT_NOT_NULL(info);
	EXPECT_EQ(64,info->size);
}

/*******************  FUNCTION  *********************/
TEST(TestSegmentTracker,testGetInvalid)
{
	//setup objects
	SegmentTracker tracker;
	SimpleCallStackNode stack(CST_STACK_1);
	
	//try to add a new entry
	tracker.add(CST_ADDR1,64,&stack);
	
	//try to get
	EXPECT_NULL(tracker.get((void*)0x41));
}

/*******************  FUNCTION  *********************/
TEST(TestSegmentTracker,testRemoveOK)
{
	//setup objects
	SegmentTracker tracker;
	SimpleCallStackNode stack(CST_STACK_1);

	//try to add a new entry
	tracker.add(CST_ADDR1,64,&stack);
	
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
