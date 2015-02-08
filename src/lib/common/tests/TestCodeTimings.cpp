/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
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
