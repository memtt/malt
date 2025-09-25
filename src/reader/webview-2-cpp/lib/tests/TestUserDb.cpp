/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 07/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/reader/webview-2-cpp/lib/tests/TestUserDb.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
#include <gtest/gtest.h>
#include "../UserDb.hpp"

/**********************************************************/
using namespace MALTWebviewCpp;

/**********************************************************/
TEST(TestUserDb, constructor)
{
	UserDb db(TEST_DATA_DIR "/db-test-constructor.json", true, false);
}

/**********************************************************/
TEST(TestUserDb, add_check)
{
	UserDb db(TEST_DATA_DIR "/db-test-add-check.json", true, false);

	//set
	db.set("toto", "tutu");
	ASSERT_TRUE(db.check("toto", "tutu"));
	ASSERT_FALSE(db.check("toto", "TUTU"));
	ASSERT_FALSE(db.check("Toto", "tutu"));

	//change
	db.set("toto", "TUTU");
	ASSERT_FALSE(db.check("toto", "tutu"));
	ASSERT_TRUE(db.check("toto", "TUTU"));
}

/**********************************************************/
TEST(TestUserDb, add_save_load_check)
{
	//remove
	unlink(TEST_DATA_DIR "/db-test-add-save-load-check.json");

	//build & save
	UserDb db(TEST_DATA_DIR "/db-test-add-save-load-check.json", true, false);
	db.set("toto", "tutu");
	db.save();

	//load & check
	UserDb db2(TEST_DATA_DIR "/db-test-add-save-load-check.json", true, false);
	ASSERT_TRUE(db2.check("toto", "tutu"));
	ASSERT_FALSE(db2.check("toto", "TUTU"));
	ASSERT_FALSE(db2.check("Toto", "tutu"));
}
