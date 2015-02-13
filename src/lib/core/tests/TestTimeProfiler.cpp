/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 02/2015
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
#include <gtest/gtest.h>
#include <core/TimeProfiler.hpp>

/***************** USING NAMESPACE ******************/
using namespace MATT;

/*********************  CONSTS  *********************/
static const char * CST_VALUE_1 =   "{\n\
\t\"steps\":1,\n\
\t\"stackIds\":[0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18],\n\
\t\"a\":[0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3],\n\
\t\"b\":[0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 0, 10, 20, 30, 40, 50, 60, 70, 80]\n}";
static const char * CST_VALUE_2 =   "{\n\
\t\"steps\":2,\n\
\t\"stackIds\":[1, 3, 5, 7, 9, 11, 13, 15, 17],\n\
\t\"a\":[1, 3, 4, 2, 4, 1, 3, 4, 2],\n\
\t\"b\":[10, 30, 50, 70, 90, 10, 30, 50, 70]\n}";
static const char * CST_VALUE_3 =   "{\n\
\t\"steps\":1,\n\
\t\"stackIds\":[0, 1, -1, -1, -1, 5, 6, 7, 8],\n\
\t\"a\":[0, 1, 1, 1, 1, 0, 1, 2, 3]\n}";
static const char * CST_VALUE_4 =   "{\n\
\t\"steps\":1,\n\
\t\"stackIds\":[0, 1, -1, -1, -1, 5, 6, 7, 8],\n\
\t\"a\":[0, 1, 0, 0, 0, 0, 1, 2, 3]\n}";



/*******************  FUNCTION  *********************/
TEST(PeakTracker,constructor)
{
	TimeProfiler profiler(2);
}

/*******************  FUNCTION  *********************/
TEST(PeakTracker,push)
{
	TimeProfiler profiler(2);
	profiler.registerEntry(0,"a");
	profiler.registerEntry(1,"b");
	for (int i = 0 ; i < 20 ; i++)
	{
		profiler.push(i,i,0,i%5);
		profiler.push(i,i,1,i%10);
	}
}

/*******************  FUNCTION  *********************/
TEST(PeakTracker,toJson)
{
	TimeProfiler profiler(2,40);
	profiler.registerEntry(0,"a");
	profiler.registerEntry(1,"b");
	for (int i = 0 ; i < 20 ; i++)
	{
		profiler.push(i,i,0,(i%5));
		profiler.push(i,i,1,(i%10)*10);
	}
	
	std::stringstream out;
	htopml::convertToJson(out,profiler);
	EXPECT_EQ(CST_VALUE_1,out.str());
}


/*******************  FUNCTION  *********************/
TEST(PeakTracker,reshape)
{
	TimeProfiler profiler(2,10);
	profiler.registerEntry(0,"a");
	profiler.registerEntry(1,"b");
	for (int i = 0 ; i < 20 ; i++)
	{
		profiler.push(i,i,0,(i%5));
		profiler.push(i,i,1,(i%10)*10);
	}
	
	std::stringstream out;
	htopml::convertToJson(out,profiler);
	EXPECT_EQ(CST_VALUE_2,out.str());
}

/*******************  FUNCTION  *********************/
TEST(PeakTracker,notTouchedTrue)
{
	TimeProfiler profiler(1,10,true);
	profiler.registerEntry(0,"a");
	for (int i = 0 ; i < 10 ; i++)
	{
		if (i < 2 || i > 4)
			profiler.push(i,i,0,(i%5));
	}
	
	std::stringstream out;
	htopml::convertToJson(out,profiler);
	EXPECT_EQ(CST_VALUE_3,out.str());
}

/*******************  FUNCTION  *********************/
TEST(PeakTracker,notTouchedFalse)
{
	TimeProfiler profiler(1,10,false);
	profiler.registerEntry(0,"a");
	for (int i = 0 ; i < 10 ; i++)
	{
		if (i < 2 || i > 4)
			profiler.push(i,i,0,(i%5));
	}
	
	std::stringstream out;
	htopml::convertToJson(out,profiler);
	EXPECT_EQ(CST_VALUE_4,out.str());
}
