/*****************************************************
             PROJECT  : MALT
             VERSION  : 1.2.2
             DATE     : 10/2020
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
#include <gtest/gtest.h>
#include <OS.hpp>

/***************** USING NAMESPACE ******************/
using namespace MALT;

/*******************  FUNCTION  *********************/
TEST(TestOS,getSignalName)
{
	EXPECT_EQ("SIGUSR1", OS::getSignalName(SIGUSR1));
	EXPECT_EQ("SIGUSR2", OS::getSignalName(SIGUSR2));
	EXPECT_EQ("SIGTERM", OS::getSignalName(SIGTERM));
	EXPECT_EQ("UNKNOWN", OS::getSignalName(-1));
}

/*******************  FUNCTION  *********************/
TEST(TestOS,getSignalFromName)
{
	EXPECT_EQ(SIGUSR1, OS::getSignalFromName("SIGUSR1"));
	EXPECT_EQ(SIGUSR2, OS::getSignalFromName("SIGUSR2"));
	EXPECT_EQ(SIGTERM, OS::getSignalFromName("SIGTERM"));
}
