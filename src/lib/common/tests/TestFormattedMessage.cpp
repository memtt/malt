/*****************************************************
             PROJECT  : MALT
             VERSION  : 1.2.2
             DATE     : 06/2023
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
#include <cerrno>
#include <gtest/gtest.h>
#include <common/FormattedMessage.hpp>
#include <common/Debug.hpp>

/***************** USING NAMESPACE ******************/
using namespace MALT;

/*******************  FUNCTION  *********************/
TEST(FormattedMessage,constructor)
{
	FormattedMessage message("Basic test");
}

/*******************  FUNCTION  *********************/
TEST(FormattedMessage,basic)
{
	FormattedMessage message("Basic test");
	EXPECT_EQ("Basic test",message.toString());
}

/*******************  FUNCTION  *********************/
TEST(FormattedMessage,string)
{
	FormattedMessage message(std::string("Basic test"));
	EXPECT_EQ("Basic test",message.toString());
}

/*******************  FUNCTION  *********************/
TEST(FormattedMessage,argFloat)
{
	FormattedMessage message("value = %1");
	message.arg(10.5);
	EXPECT_EQ("value = 10.5",message.toString());
}

/*******************  FUNCTION  *********************/
TEST(FormattedMessage,argString)
{
	std::string val = "test";
	FormattedMessage message("value = %1");
	message.arg(val);
	EXPECT_EQ("value = test",message.toString());
}

/*******************  FUNCTION  *********************/
TEST(FormattedMessage,argCString)
{
	FormattedMessage message("value = %1");
	message.arg("test");
	EXPECT_EQ("value = test",message.toString());
}

/*******************  FUNCTION  *********************/
TEST(FormattedMessage,multiArg)
{
	FormattedMessage message("value = %1, %2, %3");
	message.arg(10);
	message.arg(20);
	message.arg(30.1);
	EXPECT_EQ("value = 10, 20, 30.1",message.toString());
}

/*******************  FUNCTION  *********************/
TEST(FormattedMessage,notEnought)
{
	FormattedMessage message("value = %1, %2, %3, %4");
	message.arg(10);
	message.arg(20);
	message.arg(30.1);
	EXPECT_EQ("value = 10, 20, 30.1, %4",message.toString());
}

/*******************  FUNCTION  *********************/
TEST(FormattedMessage,notTooMuch)
{
	FormattedMessage message("value = %1, %2, %3");
	message.arg(10);
	message.arg(20);
	message.arg(30.1);
	message.arg(40);
	EXPECT_EQ("value = 10, 20, 30.1",message.toString());
}

/*******************  FUNCTION  *********************/
TEST(FormattedMessage,operatorStream)
{
	FormattedMessage message("test %1");
	message.arg("test");
	
	std::stringstream out;
	out << message;
	
	EXPECT_EQ("test test",out.str());
}

/*******************  FUNCTION  *********************/
TEST(FormattedMessage,errnoToString)
{
	FormattedMessage message("test %1");
	errno=0;
	message.argStrErrno();
	
	EXPECT_EQ("test Success",message.toString());
}
