/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 02/2015
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
#include <gtest/gtest.h>
#include <core/PeakTracker.hpp>

/***************** USING NAMESPACE ******************/
using namespace MATT;

/*********************  CONSTS  *********************/
const ssize_t CST_SEQ_1[] = {16,16,-16,16,-16,-16,32,32,-64};
const ssize_t CST_SEQ_1_CUR[] = {16,32,16,32,16,0,32,64,0};
const ssize_t CST_SEQ_1_PEAK[] = {16,32,32,32,32,32,32,64,64};
const ssize_t CST_SEQ_1_PEAKID[] = {1,2,2,2,2,2,2,3,3};
const ssize_t CST_SEQ_1_PEAKTIME[] = {0,1,1,1,1,1,1,7,7};
const size_t CST_SEQ_1_CNT = sizeof(CST_SEQ_1)/sizeof(*CST_SEQ_1);
const char * CST_SEQ_1_JSON = "{\n\t\"finalValue\":0,\n\t\"peakTime\":7,\n\t\"peak\":64\n}";
const char * CST_SEQ_1_STACK_JSON =   "{\n\t\"finalValue\":0,\n\t\"localPeakTime\":7,\n\t\"localPeak\":64,\n\t\"onGlobalPeakTime\":64\n}";



/*******************  FUNCTION  *********************/
TEST(PeakTracker,constructor)
{
	PeakTracker peak;
}

/*******************  FUNCTION  *********************/
TEST(PeakTracker,test1)
{
	PeakTracker peak;
	for (int i = 0 ; i < CST_SEQ_1_CNT ; i++)
	{
		peak.update(i,CST_SEQ_1[i]);
		EXPECT_EQ(CST_SEQ_1_CUR[i],peak.getCurrent());
		EXPECT_EQ(CST_SEQ_1_PEAK[i],peak.getPeakValue());
		EXPECT_EQ(CST_SEQ_1_PEAKID[i],peak.getPeakId());
		EXPECT_EQ(CST_SEQ_1_PEAKTIME[i],peak.getPeakTime());
	}
	
	std::stringstream out;
	htopml::convertToJson(out,peak);
	EXPECT_EQ(CST_SEQ_1_JSON,out.str());
}

/*******************  FUNCTION  *********************/
TEST(StackPeakTracker,constructor)
{
	StackPeakTracker peak;
}

/*******************  FUNCTION  *********************/
TEST(StackPeakTracker,test1)
{
	PeakTracker peak;
	StackPeakTracker stackPeak;
	for (int i = 0 ; i < CST_SEQ_1_CNT ; i++)
	{
		peak.update(i,CST_SEQ_1[i]);
		EXPECT_EQ(CST_SEQ_1_CUR[i],peak.getCurrent());
		EXPECT_EQ(CST_SEQ_1_PEAK[i],peak.getPeakValue());
		EXPECT_EQ(CST_SEQ_1_PEAKID[i],peak.getPeakId());
		EXPECT_EQ(CST_SEQ_1_PEAKTIME[i],peak.getPeakTime());
		stackPeak.update(i,CST_SEQ_1[i],peak);
	}
	
	std::stringstream out;
	htopml::convertToJson(out,stackPeak);
	EXPECT_EQ(CST_SEQ_1_STACK_JSON,out.str());
}
