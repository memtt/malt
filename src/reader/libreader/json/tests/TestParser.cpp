/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 07/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/reader/libreader/json/tests/TestParser.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
#include <gtest/gtest.h>
#include "../Parser.hpp"

/**********************************************************/
using namespace MALTJson;

/**********************************************************/
TEST(TestJsonParser, forwardWhite)
{
	const JsonString data(" \n\t {}");
	std::string fname = "main.json";
	JsonFileCursor cursor{&fname, &data};

	cursor.forwardWhite();
	ASSERT_EQ(cursor.offset, 4);

	cursor.forwardWhite();
	ASSERT_EQ(cursor.offset, 4);
}

/**********************************************************/
TEST(TestJsonParser, raiseError)
{
	const JsonString data("{\n}");
	std::string fname = "main.json";
	JsonFileCursor cursor{&fname, &data};

	ASSERT_THROW(cursor.raiseError("Test error"), JsonException);
}

/**********************************************************/
TEST(TestJsonParser, getNextChar)
{
	const JsonString data("{\n}");
	std::string fname = "main.json";
	JsonFileCursor cursor{&fname, &data};
	ASSERT_EQ(data.len, 3);

	ASSERT_EQ(cursor.getCurrent(), '{');
	ASSERT_EQ(cursor.getNextChar(false), '\n');
	ASSERT_EQ(cursor.offset, 0);
	ASSERT_EQ(cursor.column, 0);
	ASSERT_EQ(cursor.line, 0);

	ASSERT_EQ(cursor.getNextChar(), '\n');
	ASSERT_EQ(cursor.offset, 1);
	ASSERT_EQ(cursor.column, 1);
	ASSERT_EQ(cursor.line, 0);

	ASSERT_EQ(cursor.getNextChar(), '}');
	ASSERT_EQ(cursor.offset, 2);
	ASSERT_EQ(cursor.column, 0);
	ASSERT_EQ(cursor.line, 1);

	ASSERT_THROW(cursor.getNextChar(), JsonException);
}

/**********************************************************/
TEST(TestJsonParser, forwardNextStart_1)
{
	const JsonString data(" [ \"test\" ]");
	std::string fname = "main.json";
	JsonFileCursor cursor{&fname, &data};

	ASSERT_EQ(cursor.forwardNextStart(), JSON_CHAR_ARRAY_START);
	ASSERT_EQ(cursor.offset, 1);
	cursor.getNextChar();
	ASSERT_EQ(cursor.forwardNextStart(), JSON_CHAR_STRING_START_END);
	ASSERT_EQ(cursor.offset, 3);
}

/**********************************************************/
TEST(TestJsonParser, forwardNextStart_2)
{
	const JsonString data(" [ true ]");
	std::string fname = "main.json";
	JsonFileCursor cursor{&fname, &data};

	ASSERT_EQ(cursor.forwardNextStart(), JSON_CHAR_ARRAY_START);
	ASSERT_EQ(cursor.offset, 1);
	cursor.getNextChar();
	ASSERT_EQ(cursor.forwardNextStart(), JSON_CHAR_BOOL_START);
	ASSERT_EQ(cursor.offset, 3);
}

/**********************************************************/
TEST(TestJsonParser, forwardNextStart_3)
{
	const JsonString data(" [ false ]");
	std::string fname = "main.json";
	JsonFileCursor cursor{&fname, &data};

	ASSERT_EQ(cursor.forwardNextStart(), JSON_CHAR_ARRAY_START);
	ASSERT_EQ(cursor.offset, 1);
	cursor.getNextChar();
	ASSERT_EQ(cursor.forwardNextStart(), JSON_CHAR_BOOL_START);
	ASSERT_EQ(cursor.offset, 3);
}

/**********************************************************/
TEST(TestJsonParser, forwardNextStart_4)
{
	const JsonString data(" [ 158 ]");
	std::string fname = "main.json";
	JsonFileCursor cursor{&fname, &data};

	ASSERT_EQ(cursor.forwardNextStart(), JSON_CHAR_ARRAY_START);
	ASSERT_EQ(cursor.offset, 1);
	cursor.getNextChar();
	ASSERT_EQ(cursor.forwardNextStart(), JSON_CHAR_STRING_DIGIT);
	ASSERT_EQ(cursor.offset, 3);
}

/**********************************************************/
TEST(TestJsonParser, forwardNextStart_5)
{
	const JsonString data(" [ {} ]");
	std::string fname = "main.json";
	JsonFileCursor cursor{&fname, &data};

	ASSERT_EQ(cursor.forwardNextStart(), JSON_CHAR_ARRAY_START);
	ASSERT_EQ(cursor.offset, 1);
	cursor.getNextChar();
	ASSERT_EQ(cursor.forwardNextStart(), JSON_CHAR_OBJECT_START);
	ASSERT_EQ(cursor.offset, 3);
}

/**********************************************************/
TEST(TestJsonParser, forwardNextStart_6)
{
	const JsonString data(" [ [] ]");
	std::string fname = "main.json";
	JsonFileCursor cursor{&fname, &data};

	ASSERT_EQ(cursor.forwardNextStart(), JSON_CHAR_ARRAY_START);
	ASSERT_EQ(cursor.offset, 1);
	cursor.getNextChar();
	ASSERT_EQ(cursor.forwardNextStart(), JSON_CHAR_ARRAY_START);
	ASSERT_EQ(cursor.offset, 3);
}

/**********************************************************/
TEST(TestJsonParser, forwardNextStart_7)
{
	const JsonString data(" [ a ]");
	std::string fname = "main.json";
	JsonFileCursor cursor{&fname, &data};

	ASSERT_EQ(cursor.forwardNextStart(), JSON_CHAR_ARRAY_START);
	ASSERT_EQ(cursor.offset, 1);
	cursor.getNextChar();
	ASSERT_THROW(cursor.forwardNextStart(), JsonException);
}

/**********************************************************/
TEST(TestJsonParser, isWhite)
{
	const JsonString data(" \n\t");
	std::string fname = "main.json";
	JsonFileCursor cursor{&fname, &data};

	ASSERT_TRUE(cursor.isWhite(' '));
	ASSERT_TRUE(cursor.isWhite('\t'));
	ASSERT_TRUE(cursor.isWhite('\n'));
	ASSERT_FALSE(cursor.isWhite('a'));
}

/**********************************************************/
TEST(TestJsonParser, parseBoolean_true)
{
	const JsonString data("true, false");
	std::string fname = "main.json";
	JsonFileCursor cursor{&fname, &data};

	JsonNode node;
	cursor.parseBoolean(node);
	ASSERT_EQ(node.type, JSON_NODE_BOOL);
	ASSERT_EQ(node.value.value, data.value);
	ASSERT_EQ(node.value.len, 4);
	ASSERT_EQ(cursor.offset, 4);
}

/**********************************************************/
TEST(TestJsonParser, parseBoolean_false)
{
	const JsonString data("false, true");
	std::string fname = "main.json";
	JsonFileCursor cursor{&fname, &data};

	JsonNode node;
	cursor.parseBoolean(node);
	ASSERT_EQ(node.type, JSON_NODE_BOOL);
	ASSERT_EQ(node.value.value, data.value);
	ASSERT_EQ(node.value.len, 5);
	ASSERT_EQ(cursor.offset, 5);
}

/**********************************************************/
TEST(TestJsonParser, parseBoolean_error)
{
	const JsonString data("10, true");
	std::string fname = "main.json";
	JsonFileCursor cursor{&fname, &data};

	JsonNode node;
	EXPECT_THROW(cursor.parseBoolean(node), JsonException);
}

/**********************************************************/
TEST(TestJsonParser, parseNumber_int)
{
	const JsonString data("1024, 1025");
	std::string fname = "main.json";
	JsonFileCursor cursor{&fname, &data};

	JsonNode node;
	cursor.parseNumber(node);
	ASSERT_EQ(node.type, JSON_NODE_INTEGER);
	ASSERT_EQ(node.value.value, data.value);
	ASSERT_EQ(node.value.len, 4);
	ASSERT_EQ(cursor.offset, 4);
}

/**********************************************************/
TEST(TestJsonParser, parseNumber_neg_int)
{
	const JsonString data("-1024, 1025");
	std::string fname = "main.json";
	JsonFileCursor cursor{&fname, &data};

	JsonNode node;
	cursor.parseNumber(node);
	ASSERT_EQ(node.type, JSON_NODE_INTEGER);
	ASSERT_EQ(node.value.value, data.value);
	ASSERT_EQ(node.value.len, 5);
	ASSERT_EQ(cursor.offset, 5);
}

/**********************************************************/
TEST(TestJsonParser, parseNumber_float)
{
	const JsonString data("1024.38, 1025");
	std::string fname = "main.json";
	JsonFileCursor cursor{&fname, &data};

	JsonNode node;
	cursor.parseNumber(node);
	ASSERT_EQ(node.type, JSON_NODE_FLOAT);
	ASSERT_EQ(node.value.value, data.value);
	ASSERT_EQ(node.value.len, 7);
	ASSERT_EQ(cursor.offset, 7);
}

/**********************************************************/
TEST(TestJsonParser, parseNumber_float_exp)
{
	const JsonString data("1024.38e-39, 1025");
	std::string fname = "main.json";
	JsonFileCursor cursor{&fname, &data};

	JsonNode node;
	cursor.parseNumber(node);
	ASSERT_EQ(node.type, JSON_NODE_FLOAT);
	ASSERT_EQ(node.value.value, data.value);
	ASSERT_EQ(node.value.len, 11);
	ASSERT_EQ(cursor.offset, 11);
}

/**********************************************************/
TEST(TestJsonParser, parseString)
{
	const JsonString data("\"message message\", 10");
	std::string fname = "main.json";
	JsonFileCursor cursor{&fname, &data};

	JsonNode node;
	cursor.parseString(node);
	ASSERT_EQ(node.type, JSON_NODE_STRING);
	ASSERT_EQ(node.value.value, data.value+1);
	ASSERT_EQ(node.value.len, 15);
	ASSERT_EQ(cursor.offset, 17);
	ASSERT_EQ(cursor.getCurrent(), ',');
}

/**********************************************************/
TEST(TestJsonParser, parseString_backslahes)
{
	const JsonString data("\"message \\\n message\", 10");
	std::string fname = "main.json";
	JsonFileCursor cursor{&fname, &data};

	JsonNode node;
	cursor.parseString(node);
	ASSERT_EQ(node.type, JSON_NODE_STRING);
	ASSERT_EQ(node.value.value, data.value+1);
	ASSERT_EQ(node.value.len, 18);
	ASSERT_EQ(cursor.offset, 20);
	ASSERT_EQ(cursor.getCurrent(), ',');
}

/**********************************************************/
TEST(TestJsonParser, parseString_empty)
{
	const JsonString data("\"\", 10");
	std::string fname = "main.json";
	JsonFileCursor cursor{&fname, &data};

	JsonNode node;
	cursor.parseString(node);
	ASSERT_EQ(node.type, JSON_NODE_STRING);
	ASSERT_EQ(node.value.value, data.value+1);
	ASSERT_EQ(node.value.len, 0);
	ASSERT_EQ(cursor.offset, 2);
	ASSERT_EQ(cursor.getCurrent(), ',');
}

/**********************************************************/
TEST(TestJsonParser, parseString_backslahes_quote)
{
	const JsonString data("\"message \\\" message\", 10");
	std::string fname = "main.json";
	JsonFileCursor cursor{&fname, &data};

	JsonNode node;
	cursor.parseString(node);
	ASSERT_EQ(node.type, JSON_NODE_STRING);
	ASSERT_EQ(node.value.value, data.value+1);
	ASSERT_EQ(node.value.len, 18);
	ASSERT_EQ(cursor.offset, 20);
	ASSERT_EQ(cursor.getCurrent(), ',');
}

/**********************************************************/
TEST(TestJsonParser, parseChild_bool)
{
	const JsonString data("true, false");
	std::string fname = "main.json";
	JsonFileCursor cursor{&fname, &data};

	JsonNode node;
	cursor.parseChild(node);
	ASSERT_EQ(node.type, JSON_NODE_BOOL);
	ASSERT_EQ(node.value.value, data.value);
	ASSERT_EQ(node.value.len, 4);
	ASSERT_EQ(cursor.offset, 4);
	ASSERT_EQ(cursor.getCurrent(), ',');
}

/**********************************************************/
TEST(TestJsonParser, parseChild_number)
{
	const JsonString data("1024.5, false");
	std::string fname = "main.json";
	JsonFileCursor cursor{&fname, &data};

	JsonNode node;
	cursor.parseChild(node);
	ASSERT_EQ(node.type, JSON_NODE_FLOAT);
	ASSERT_EQ(node.value.value, data.value);
	ASSERT_EQ(node.value.len, 6);
	ASSERT_EQ(cursor.offset, 6);
	ASSERT_EQ(cursor.getCurrent(), ',');
}

/**********************************************************/
TEST(TestJsonParser, parseChild_string)
{
	const JsonString data("\"value\", false");
	std::string fname = "main.json";
	JsonFileCursor cursor{&fname, &data};

	JsonNode node;
	cursor.parseChild(node);
	ASSERT_EQ(node.type, JSON_NODE_STRING);
	ASSERT_EQ(node.value.value, data.value+1);
	ASSERT_EQ(node.value.len, 5);
	ASSERT_EQ(cursor.offset, 7);
	ASSERT_EQ(cursor.getCurrent(), ',');
}

/**********************************************************/
TEST(TestJsonParser, parseArray_int)
{
	const JsonString data("[ 10, 20 , 30 ]\n");
	std::string fname = "main.json";
	JsonFileCursor cursor{&fname, &data};

	JsonNode node;
	cursor.parseArray(node);
	ASSERT_EQ(node.type, JSON_NODE_ARRAY);
	ASSERT_EQ(node.value.value, data.value);
	ASSERT_EQ(node.value.len, 15);
	ASSERT_EQ(cursor.offset, 15);
	ASSERT_EQ(cursor.getCurrent(), '\n');

	ASSERT_EQ(node.view.vector->size(), 3);
	ASSERT_EQ((*node.view.vector)[0].type, JSON_NODE_INTEGER);
	ASSERT_EQ((*node.view.vector)[0].value.value, data.value+2);
	ASSERT_EQ((*node.view.vector)[1].type, JSON_NODE_INTEGER);
	ASSERT_EQ((*node.view.vector)[1].value.value, data.value+6);
	ASSERT_EQ((*node.view.vector)[2].type, JSON_NODE_INTEGER);
	ASSERT_EQ((*node.view.vector)[2].value.value, data.value+11);
}

/**********************************************************/
TEST(TestJsonParser, parseArray_mix)
{
	const JsonString data("[ 10, true , \"abc\" ]\n");
	std::string fname = "main.json";
	JsonFileCursor cursor{&fname, &data};

	JsonNode node;
	cursor.parseArray(node);
	ASSERT_EQ(node.type, JSON_NODE_ARRAY);
	ASSERT_EQ(node.value.value, data.value);
	ASSERT_EQ(node.value.len, 20);
	ASSERT_EQ(cursor.offset, 20);
	ASSERT_EQ(cursor.getCurrent(), '\n');

	ASSERT_EQ(node.view.vector->size(), 3);
	ASSERT_EQ((*node.view.vector)[0].type, JSON_NODE_INTEGER);
	ASSERT_EQ((*node.view.vector)[0].value.value, data.value+2);
	ASSERT_EQ((*node.view.vector)[1].type, JSON_NODE_BOOL);
	ASSERT_EQ((*node.view.vector)[1].value.value, data.value+6);
	ASSERT_EQ((*node.view.vector)[2].type, JSON_NODE_STRING);
	ASSERT_EQ((*node.view.vector)[2].value.value, data.value+14);
}

/**********************************************************/
TEST(TestJsonParser, parseArray_mix_no_white)
{
	const JsonString data("[10,true,\"abc\"]\n");
	std::string fname = "main.json";
	JsonFileCursor cursor{&fname, &data};

	JsonNode node;
	cursor.parseArray(node);
	ASSERT_EQ(node.type, JSON_NODE_ARRAY);
	ASSERT_EQ(node.value.value, data.value);
	ASSERT_EQ(node.value.len, 15);
	ASSERT_EQ(cursor.offset, 15);
	ASSERT_EQ(cursor.getCurrent(), '\n');

	ASSERT_EQ(node.view.vector->size(), 3);
	ASSERT_EQ((*node.view.vector)[0].type, JSON_NODE_INTEGER);
	ASSERT_EQ((*node.view.vector)[0].value.value, data.value+1);
	ASSERT_EQ((*node.view.vector)[1].type, JSON_NODE_BOOL);
	ASSERT_EQ((*node.view.vector)[1].value.value, data.value+4);
	ASSERT_EQ((*node.view.vector)[2].type, JSON_NODE_STRING);
	ASSERT_EQ((*node.view.vector)[2].value.value, data.value+10);
}

/**********************************************************/
TEST(TestJsonParser, parseArray_mix_no_white_recurse)
{
	const JsonString data("[10,true,\"abc\",[25,26] ]\n");
	std::string fname = "main.json";
	JsonFileCursor cursor{&fname, &data};

	JsonNode node;
	cursor.parseArray(node);
	ASSERT_EQ(node.type, JSON_NODE_ARRAY);
	ASSERT_EQ(node.value.value, data.value);
	ASSERT_EQ(node.value.len, 24);
	ASSERT_EQ(cursor.offset, 24);
	ASSERT_EQ(cursor.getCurrent(), '\n');

	ASSERT_EQ(node.view.vector->size(), 4);
	ASSERT_EQ((*node.view.vector)[0].type, JSON_NODE_INTEGER);
	ASSERT_EQ((*node.view.vector)[0].value.value, data.value+1);
	ASSERT_EQ((*node.view.vector)[1].type, JSON_NODE_BOOL);
	ASSERT_EQ((*node.view.vector)[1].value.value, data.value+4);
	ASSERT_EQ((*node.view.vector)[2].type, JSON_NODE_STRING);
	ASSERT_EQ((*node.view.vector)[2].value.value, data.value+10);
	ASSERT_EQ((*node.view.vector)[3].type, JSON_NODE_ARRAY);
	ASSERT_EQ((*node.view.vector)[3].value.value, data.value+15);
}

/**********************************************************/
TEST(TestJsonParser, parseObject_int)
{
	const JsonString data("{ \"a\": 10, \"b\": 20 , \"c\": 30 }\n");
	std::string fname = "main.json";
	JsonFileCursor cursor{&fname, &data};

	JsonNode node;
	cursor.parseObject(node);
	ASSERT_EQ(node.type, JSON_NODE_OBJECT);
	ASSERT_EQ(node.value.value, data.value);
	ASSERT_EQ(node.value.len, 29);
	ASSERT_EQ(cursor.offset, 30);
	ASSERT_EQ(cursor.getCurrent(), '\n');

	ASSERT_EQ(node.view.map->size(), 3);
	ASSERT_EQ((*node.view.map)["a"].type, JSON_NODE_INTEGER);
	ASSERT_EQ((*node.view.map)["a"].value.value, data.value+7);
	ASSERT_EQ((*node.view.map)["b"].type, JSON_NODE_INTEGER);
	ASSERT_EQ((*node.view.map)["b"].value.value, data.value+16);
	ASSERT_EQ((*node.view.map)["c"].type, JSON_NODE_INTEGER);
	ASSERT_EQ((*node.view.map)["c"].value.value, data.value+26);
}

/**********************************************************/
TEST(TestJsonParser, parseObject_recurse)
{
	const JsonString data("{ \"a\": 10, \"b\": 20 , \"c\": {\"z\": 55}\n} ");
	std::string fname = "main.json";
	JsonFileCursor cursor{&fname, &data};

	JsonNode node;
	cursor.parseObject(node);
	ASSERT_EQ(node.type, JSON_NODE_OBJECT);
	ASSERT_EQ(node.value.value, data.value);
	ASSERT_EQ(node.value.len, 36);
	ASSERT_EQ(cursor.offset, 37);
	ASSERT_EQ(cursor.getCurrent(), ' ');

	ASSERT_EQ(node.view.map->size(), 3);
	ASSERT_EQ((*node.view.map)["a"].type, JSON_NODE_INTEGER);
	ASSERT_EQ((*node.view.map)["a"].value.value, data.value+7);
	ASSERT_EQ((*node.view.map)["b"].type, JSON_NODE_INTEGER);
	ASSERT_EQ((*node.view.map)["b"].value.value, data.value+16);
	ASSERT_EQ((*node.view.map)["c"].type, JSON_NODE_OBJECT);
	ASSERT_EQ((*node.view.map)["c"].value.value, data.value+26);
}

/**********************************************************/
TEST(TestJsonParser, parse)
{

}