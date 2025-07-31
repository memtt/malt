#include <gtest/gtest.h>
#include "../Json.hpp"

using namespace MALTJson;

TEST(TestJson, iterate_array)
{
	JsonRoot root = parse("[10, 20, 30, 40]");
	ASSERT_EQ(root[0], 10);
	ASSERT_EQ(root[1], 20);
	ASSERT_EQ(root[2], 30);
	ASSERT_EQ(root[3], 40);
}
