/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MATT_GTEST_HELPER_HPP
#define MATT_GTEST_HELPER_HPP

/********************  HEADERS  *********************/
#include <gtest/gtest.h>
	
/*******************  NAMESPACE  ********************/
namespace MATT
{

/********************  MACROS  **********************/
#define EXPECT_NULL(x) EXPECT_EQ((void*)NULL,(x))
#define EXPECT_NOT_NULL(x) EXPECT_NE((void*)NULL,(x))
#define ASSERT_NULL(x) ASSERT_EQ((void*)NULL,(x))
#define ASSERT_NOT_NULL(x) ASSERT_NE((void*)NULL,(x))

}

#endif //MATT_GTEST_HELPER_HPP
