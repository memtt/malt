/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 08/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/common/GTestHelper.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2014
*    AUTHOR   : Sébastien Valat (ECR) - 2014
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

#ifndef MALT_GTEST_HELPER_HPP
#define MALT_GTEST_HELPER_HPP

/**********************************************************/
#include <gtest/gtest.h>
	
/**********************************************************/
namespace MALT
{

/**********************************************************/
/**
 * Extend the EXPECT_EQ to check nullity with right casting.
 */
#define EXPECT_NULL(x) EXPECT_EQ((void*)NULL,(x))
/**
 * Extend the EXPECT_NE to check non nullity with right casting.
 */
#define EXPECT_NOT_NULL(x) EXPECT_NE((void*)NULL,(x))
/**
 * Extend the ASSERT_EQ to check nullity with right casting.
 */
#define ASSERT_NULL(x) ASSERT_EQ((void*)NULL,(x))
/**
 * Extend the ASSERT_NE to check non nullity with right casting.
 */
#define ASSERT_NOT_NULL(x) ASSERT_NE((void*)NULL,(x))

}

#endif //MALT_GTEST_HELPER_HPP
