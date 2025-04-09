/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 10/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/core/tests/TestAllocTraceFile.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2022 - 2024
***********************************************************/

/**********************************************************/
#include <gtest/gtest.h>
#include "../FuncMetrics.hpp"

/**********************************************************/
using namespace MALTReader;

/**********************************************************/
TEST(TestFuncMetrics, constructor)
{
	MaltFuncMetrics metrics;
}

/**********************************************************/
TEST(TestFuncMetrics, getMetrics)
{
	MaltFuncMetrics metrics;
	const std::map<std::string, MaltMetric> & m = metrics.getMetrics();
	ASSERT_NE(0, m.size());
}

/**********************************************************/
TEST(TestFuncMetrics, getMetricNames)
{
	MaltFuncMetrics metrics;
	std::vector<std::string> expectedNames{
		 "alloc.count",
		 "alloc.max",
		 "alloc.mean",
		 "alloc.min",
		 "alloc.sum",
		 "free.count",
		 "free.sum",
		 "leaks",
		 "lifetime.max",
		 "lifetime.min",
		 "memops",
		 "peakmem.global",
		 "peakmem.local",
		 "realloc.count",
		 "realloc.sum",
		 "recycling.ratio"
	};
	ASSERT_EQ(expectedNames, metrics.getMetricNames());
}

/**********************************************************/
TEST(TestFuncMetrics, getValue_alloc_count)
{
	MaltFuncMetrics metrics;
	FlatProfileValue dataElement;
	dataElement.own.alloc.count = 100;
	dataElement.total.alloc.count = 200;
	ASSERT_EQ(100, metrics.getValue(dataElement, "alloc.count", false));
	ASSERT_EQ(200, metrics.getValue(dataElement, "alloc.count", true));
}

/**********************************************************/
TEST(TestFuncMetrics, getFormattedValue_alloc_count)
{
	MaltFuncMetrics metrics;
	FlatProfileValue dataElement;
	dataElement.own.alloc.count = 100;
	dataElement.total.alloc.count = 200;
	ASSERT_EQ("100  ", metrics.getFormattedValue(dataElement, "alloc.count", false));
	ASSERT_EQ("200  ", metrics.getFormattedValue(dataElement, "alloc.count", true));
}

/**********************************************************/
TEST(TestFuncMetrics, getFormattedValue_alloc_sum)
{
	MaltFuncMetrics metrics;
	FlatProfileValue dataElement;
	dataElement.own.alloc.sum = 1024*1024;
	dataElement.total.alloc.sum = 4096*10*1024;
	ASSERT_EQ("1.0 MB", metrics.getFormattedValue(dataElement, "alloc.sum", false));
	ASSERT_EQ("40.0 MB", metrics.getFormattedValue(dataElement, "alloc.sum", true));
}