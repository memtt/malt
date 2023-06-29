/*****************************************************
             PROJECT  : MALT
             VERSION  : 1.2.2
             DATE     : 06/2023
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
#include <gtest/gtest.h>
#include <core/CallStackInfo.hpp>
#include <json/JsonState.h>

/***************** USING NAMESPACE ******************/
using namespace MALT;

/*******************  FUNCTION  *********************/
TEST(TestSimpleQuantityHistory, constructor)
{
	//build
	SimpleQuantityHistory history;

	//check
	EXPECT_EQ(0, history.count);
	EXPECT_EQ(0, history.min);
	EXPECT_EQ(0, history.max);
	EXPECT_EQ(0, history.sum);
}

/*******************  FUNCTION  *********************/
TEST(TestSimpleQuantityHistory, addEvent)
{
	//build
	SimpleQuantityHistory history;

	//call
	history.addEvent(32);
	history.addEvent(32);
	history.addEvent(64);

	//check
	EXPECT_EQ(3, history.count);
	EXPECT_EQ(32, history.min);
	EXPECT_EQ(64, history.max);
	EXPECT_EQ(128, history.sum);
}

/*******************  FUNCTION  *********************/
TEST(TestSimpleQuantityHistory, push)
{
	//build
	SimpleQuantityHistory history;
	SimpleQuantityHistory subHistory;

	//call
	subHistory.addEvent(32);
	subHistory.addEvent(32);
	subHistory.addEvent(64);

	//merge two times
	history.push(subHistory);
	subHistory.addEvent(128);
	subHistory.addEvent(128);
	subHistory.addEvent(16);
	history.push(subHistory);

	//check
	EXPECT_EQ(9, history.count);
	EXPECT_EQ(16, history.min);
	EXPECT_EQ(128, history.max);
	EXPECT_EQ(528, history.sum);
}

/*******************  FUNCTION  *********************/
TEST(TestSimpleQuantityHistory, getMean)
{
	//build
	SimpleQuantityHistory history;

	//check
	EXPECT_EQ(0, history.getMean());

	//call
	history.addEvent(32);
	history.addEvent(32);
	history.addEvent(64);

	//check
	EXPECT_EQ(42, history.getMean());
}

/*******************  FUNCTION  *********************/
TEST(TestCallStackInfo, constructor)
{
	CallStackInfo info;
}

/*******************  FUNCTION  *********************/
TEST(TestCallStackInfo, onAllocEvent)
{
	//build
	CallStackInfo info;

	//call
	info.onAllocEvent(64, 0);
	info.onAllocEvent(64, 1);
	info.onAllocEvent(128, 2);

	//call
	const SimpleQuantityHistory & history = info.getAllocInfo();

	//check
	EXPECT_EQ(3, history.count);
	EXPECT_EQ(64, history.min);
	EXPECT_EQ(128, history.max);
	EXPECT_EQ(256, history.sum);
}

/*******************  FUNCTION  *********************/
TEST(TestCallStackInfo, onFreeEvent)
{
	//build
	CallStackInfo info;

	//call
	info.onFreeEvent(64, 0);
	info.onFreeEvent(64, 0);
	info.onFreeEvent(128, 0);
	info.onFreeEvent(0, 0);

	//call
	const SimpleQuantityHistory & history = info.getFreeInfo();

	//check
	EXPECT_EQ(3, history.count);
	EXPECT_EQ(64, history.min);
	EXPECT_EQ(128, history.max);
	EXPECT_EQ(256, history.sum);
}

/*******************  FUNCTION  *********************/
TEST(TestCallStackInfo, onFreeLinkedMemory)
{
	//build
	CallStackInfo info;

	//call
	info.onFreeLinkedMemory(-64, 1000, 1);
	info.onFreeLinkedMemory(-64, 100, 2);
	info.onFreeLinkedMemory(-128, 4000, 3);

	//call
	const SimpleQuantityHistory & history = info.getLifetime();

	//check
	EXPECT_EQ(3, history.count);
	EXPECT_EQ(100, history.min);
	EXPECT_EQ(4000, history.max);
	EXPECT_EQ(5100, history.sum);
}

/*******************  FUNCTION  *********************/
TEST(TestCallStackInfo, merge)
{
	//build
	CallStackInfo info;

	//call
	info.onAllocEvent(64, 0);
	info.onAllocEvent(64, 0);
	info.onAllocEvent(128, 0);
	info.onFreeEvent(64, 0);
	info.onFreeEvent(64, 0);
	info.onFreeEvent(128, 0);
	info.onFreeLinkedMemory(-64, 1000, 1);
	info.onFreeLinkedMemory(-64, 100, 2);
	info.onFreeLinkedMemory(-128, 4000, 3);


	//build
	CallStackInfo info2;
	info2.merge(info);
	info2.merge(info);

	//call
	const SimpleQuantityHistory & historyAlloc = info2.getAllocInfo();

	//check
	EXPECT_EQ(6, historyAlloc.count);
	EXPECT_EQ(64, historyAlloc.min);
	EXPECT_EQ(128, historyAlloc.max);
	EXPECT_EQ(512, historyAlloc.sum);

	//call
	const SimpleQuantityHistory & historyFree = info2.getFreeInfo();

	//check
	EXPECT_EQ(6, historyFree.count);
	EXPECT_EQ(64, historyFree.min);
	EXPECT_EQ(128, historyFree.max);
	EXPECT_EQ(512, historyFree.sum);

	//call
	const SimpleQuantityHistory & historyLife = info2.getLifetime();

	//check
	EXPECT_EQ(6, historyLife.count);
	EXPECT_EQ(100, historyLife.min);
	EXPECT_EQ(4000, historyLife.max);
	EXPECT_EQ(10200, historyLife.sum);
}

/*******************  FUNCTION  *********************/
TEST(TestCallStackInfo, onReallocEvent)
{
	//build
	CallStackInfo info;

	//check
	EXPECT_FALSE(info.hasRealloc());

	//call
	info.onReallocEvent(64, 128);
	info.onReallocEvent(128, 64);
	
	//check
	EXPECT_TRUE(info.hasRealloc());
}

/*******************  FUNCTION  *********************/
TEST(TestCallStackInfo, writeAsCallgrindCallEntry)
{
	//build
	CallStackInfo info;

	//call
	info.onAllocEvent(32, 1);
	info.onAllocEvent(64, 2);
	info.onFreeEvent(32, 3);
	info.onFreeEvent(64, 3);

	//convert
	std::stringstream out;
	info.writeAsCallgrindCallEntry(10, out);
	EXPECT_EQ(out.str(), "calls=4 0\n");

	//convert
	std::stringstream out2;
	info.writeAsCallgrindEntry(10, out2);
	EXPECT_EQ(out2.str(), "10 2 2 4 96 96 96 96");
}

/*******************  FUNCTION  *********************/
TEST(TestCallStackInfo, updatePeak)
{
	//build
	CallStackInfo info;
	info.onAllocEvent(64, 0);
	info.updatePeak(0);
	info.onAllocEvent(64, 1);
	info.updatePeak(1);
	info.onAllocEvent(64, 2);
	info.updatePeak(2);
	info.onFreeEvent(64, 3);
	info.updatePeak(3);
	EXPECT_EQ(info.getPeak(), 192);
}

/*******************  FUNCTION  *********************/
TEST(TestCallStackInfo, convertToJson)
{
	//build
	CallStackInfo info;

	//call
	info.onAllocEvent(64, 0);
	info.onAllocEvent(64, 0);
	info.onAllocEvent(128, 0);
	info.onFreeEvent(64, 0);
	info.onFreeEvent(64, 0);
	info.onFreeEvent(128, 0);
	info.onFreeLinkedMemory(-64, 1000, 1);
	info.onFreeLinkedMemory(-64, 100, 2);
	info.onFreeLinkedMemory(-128, 4000, 3);

	//convert
	std::stringstream out;
	htopml::convertToJson(out, info);
}