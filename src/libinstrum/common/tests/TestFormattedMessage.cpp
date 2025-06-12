/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.6
*    DATE     : 11/2014
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/common/tests/TestFormattedMessage.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (ECR) - 2014
*    AUTHOR   : Sébastien Valat - 2014
***********************************************************/

/**********************************************************/
#include <cerrno>
#include <gtest/gtest.h>
#include <common/FormattedMessage.hpp>
#include <common/Debug.hpp>

/**********************************************************/
using namespace MALT;

/**********************************************************/
TEST(FormattedMessage,constructor)
{
	FormattedMessage message("Basic test");
}

/**********************************************************/
TEST(FormattedMessage,basic)
{
	FormattedMessage message("Basic test");
	EXPECT_EQ("Basic test",message.toString());
}

/**********************************************************/
TEST(FormattedMessage,string)
{
	FormattedMessage message(std::string("Basic test"));
	EXPECT_EQ("Basic test",message.toString());
}

/**********************************************************/
TEST(FormattedMessage,argFloat)
{
	FormattedMessage message("value = %1");
	message.arg(10.5);
	EXPECT_EQ("value = 10.5",message.toString());
}

/**********************************************************/
TEST(FormattedMessage,argString)
{
	std::string val = "test";
	FormattedMessage message("value = %1");
	message.arg(val);
	EXPECT_EQ("value = test",message.toString());
}

/**********************************************************/
TEST(FormattedMessage,argCString)
{
	FormattedMessage message("value = %1");
	message.arg("test");
	EXPECT_EQ("value = test",message.toString());
}

/**********************************************************/
TEST(FormattedMessage,multiArg)
{
	FormattedMessage message("value = %1, %2, %3");
	message.arg(10);
	message.arg(20);
	message.arg(30.1);
	EXPECT_EQ("value = 10, 20, 30.1",message.toString());
}

/**********************************************************/
TEST(FormattedMessage,notEnought)
{
	FormattedMessage message("value = %1, %2, %3, %4");
	message.arg(10);
	message.arg(20);
	message.arg(30.1);
	EXPECT_EQ("value = 10, 20, 30.1, %4",message.toString());
}

/**********************************************************/
TEST(FormattedMessage,notTooMuch)
{
	FormattedMessage message("value = %1, %2, %3");
	message.arg(10);
	message.arg(20);
	message.arg(30.1);
	message.arg(40);
	EXPECT_EQ("value = 10, 20, 30.1",message.toString());
}

/**********************************************************/
TEST(FormattedMessage,operatorStream)
{
	FormattedMessage message("test %1");
	message.arg("test");
	
	std::stringstream out;
	out << message;
	
	EXPECT_EQ("test test",out.str());
}

/**********************************************************/
TEST(FormattedMessage,errnoToString)
{
	FormattedMessage message("test %1");
	errno=0;
	message.argStrErrno();
	
	EXPECT_EQ("test Success",message.toString());
}
