/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 02/2015
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
#include <gtest/gtest.h>
#include <common/CodeTiming.hpp>

/***************** USING NAMESPACE ******************/
using namespace MATT;

/*******************  FUNCTION  *********************/
TEST(TestOptions,test_macro_code_timing)
{
	int a;
	CODE_TIMING("test_macro_code_timing",a=2+2);
}

/*******************  FUNCTION  *********************/
TEST(TestOptions,test_macro_start_stop)
{
	int a;
	CODE_TIMING_FUNC_START("test_macro_start_stop");
	a=2+2;
	CODE_TIMING_FUNC_STOP("test_macro_start_stop");
}

/*******************  FUNCTION  *********************/
TEST(TestOptions,test_object)
{
	CodeTiming timing("test");
	EXPECT_EQ("test",timing.getName());
	
}

/*******************  FUNCTION  *********************/
TEST(TestOptions,test_start_stop)
{
	CodeTiming timing("test");
	timing.start();
	timing.stop(100);
}

/*******************  FUNCTION  *********************/
TEST(TestOptions,test_final_print)
{
	CodeTiming timing("test");
	timing.start();
	timing.stop(100);
	std::stringstream out;
	timing.finalPrint(out);
	#ifdef MATT_ENABLE_CODE_TIMING
	EXPECT_EQ("",out.str());
	#else
	EXPECT_EQ("",out.str());
	#endif
}
