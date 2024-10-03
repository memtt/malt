/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 10/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/common/GTestHelper.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2014 - 2024
*    AUTHOR   : Sébastien Valat (ECR) - 2014
***********************************************************/

#ifndef MALT_GTEST_HELPER_HPP
#define MALT_GTEST_HELPER_HPP

/**********************************************************/
#include <gtest/gtest.h>
	
/**********************************************************/
namespace MALT
{

/**********************************************************/
#define EXPECT_NULL(x) EXPECT_EQ((void*)NULL,(x))
#define EXPECT_NOT_NULL(x) EXPECT_NE((void*)NULL,(x))
#define ASSERT_NULL(x) ASSERT_EQ((void*)NULL,(x))
#define ASSERT_NOT_NULL(x) ASSERT_NE((void*)NULL,(x))

}

#endif //MALT_GTEST_HELPER_HPP
