/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 07/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/reader/libreader/json/Parser.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
#include <cassert>
#include <sstream>
#include <list>
#include <cstring>
#include "Parser.hpp"

/**********************************************************/
namespace MALTJson
{

/**********************************************************/
void JsonFileCursor::raiseError(const std::string & message) const
{
	std::stringstream out;
	out << *this->fname << ":" << this->line << ":" << this->column << " : " << message;
	throw JsonException(out.str());
}

/**********************************************************/
JsonCharType JsonFileCursor::forwardNextStart(void)
{
	//skip white
	this->forwardWhite();

	//case
	switch (this->getCurrent()) {
		case '{':
			return JSON_CHAR_OBJECT_START;
		case '}':
			return JSON_CHAR_OBJECT_END;
		case '[':
			return JSON_CHAR_ARRAY_START;
		case ']':
			return JSON_CHAR_ARRAY_END;
		case '"':
			return JSON_CHAR_STRING_START_END;
		case ',':
			return JSON_CHAR_SEPARATOR;
		case 't':
		case 'f':
			return JSON_CHAR_BOOL_START;
		case ':':
			return JSON_CHAR_FIELD_SEP;
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		case '-':
			return JSON_CHAR_STRING_DIGIT;
		default:
			this->raiseError("Unexpected character !");
			return JSON_CHAR_ERROR;
	}
}

/**********************************************************/
void JsonFileCursor::parseObject(JsonNode & node)
{
	//check
	assert(this->getCurrent() == '{');

	//add
	node.type = JSON_NODE_OBJECT;
	node.value.value = this->data->value + this->offset;

	//move
	size_t startOffset = this->offset;
	this->getNextChar();

	//map
	const size_t block = 10240;
	std::list<std::pair<JsonString, JsonNode> > tmpMap;
	//tmpMap.reserve(block);

	while (1) {
		//vars
		JsonCharType next = this->forwardNextStart();

		//is end
		if (next == JSON_CHAR_OBJECT_END)
			break;
		if (tmpMap.empty() == false) {
			if (this->forwardNextStart() != JSON_CHAR_SEPARATOR)
				this->raiseError("Expect field separator (,) !");
			this->getNextChar();
			next = this->forwardNextStart();
		}

		//should be string
		if (next != JSON_CHAR_STRING_START_END)
			this->raiseError("Expect a string as object field name !");

		//parse string
		JsonNode key;
		this->parseString(key);

		//vars
		next = this->forwardNextStart();

		//should be :
		if (next != JSON_CHAR_FIELD_SEP)
			this->raiseError("Expecte field separator (:) !");

		//forward
		this->getNextChar();
		next = this->forwardNextStart();

		//parse it
		JsonNode child;
		this->parseChild(child);

		//set
		assert(key.type == JSON_NODE_STRING);
		tmpMap.emplace_back(std::pair<JsonString, JsonNode>{key.value, std::move(child)});
		//(*object)[key.value] = child;
		//fprintf(stderr, "%s\n", key.value.toString().c_str());

		//reset
		child.type = JSON_NODE_NONE;
		child.view.map = nullptr;
	}

	//copy
	this->pushObjectChilds(node, tmpMap);

	//store
	node.value.len = this->offset - startOffset;

	//move next
	this->getNextChar();
}

/**********************************************************/
void JsonFileCursor::pushObjectChilds(JsonNode & node, std::vector<std::pair<JsonString, JsonNode> > & childs)
{
	//copy
	if (node.view.vmap == nullptr)
		node.view.vmap = new std::vector<std::pair<JsonString, JsonNode> >();
	node.view.vmap->reserve(childs.size());
	for (auto & it : childs)
		node.view.vmap->emplace_back(std::move(it));
	childs.clear();
	assert(childs.size() == 0);
}

/**********************************************************/
void JsonFileCursor::pushObjectChilds(JsonNode & node, std::list<std::pair<JsonString, JsonNode> > & childs)
{
	//copy
	if (node.view.vmap == nullptr)
		node.view.vmap = new std::vector<std::pair<JsonString, JsonNode> >();
	node.view.vmap->reserve(childs.size());
	for (auto & it : childs)
		node.view.vmap->emplace_back(std::move(it));
	childs.clear();
}

/**********************************************************/
void JsonFileCursor::parseArray(JsonNode & node)
{
	//check
	assert(this->getCurrent() == '[');

	//store
	node.value.value = this->data->value + this->offset;
	size_t startOffset = this->offset;

	//move
	this->getNextChar();

	//vars
	JsonCharType next = this->forwardNextStart();
	std::list<JsonNode> childs;

	//loop all
	while (next != JSON_CHAR_ARRAY_END) {
		//add new child
		childs.emplace_back();
		JsonNode & child = childs.back();

		//parse it
		this->parseChild(child);

		//get next
		next = this->forwardNextStart();

		//is coma
		if (next == JSON_CHAR_SEPARATOR) {
			this->getNextChar();
			next = this->forwardNextStart();
			if (next == JSON_CHAR_ARRAY_END)
				this->raiseError("Invalid comma befre en of array !");
		}
	}

	//copy
	node.type = JSON_NODE_ARRAY;
	node.view.vector = new std::vector<JsonNode>();

	//prealloc & move
	node.view.vector->reserve(childs.size());
	for (auto & it : childs) {
		node.view.vector->emplace_back(it);
		it.type = JSON_NODE_NONE;
		it.view.vector = nullptr;
	}

	//set size
	node.value.len = this->offset - startOffset + 1;

	//move next
	this->getNextChar();
}

/**********************************************************/
void JsonFileCursor::parseString(JsonNode & node)
{
	//check
	assert(this->getCurrent() == '"');
	char next = this->getNextChar();

	//register start
	node.type = JSON_NODE_STRING;
	node.value.value = this->data->value + this->offset;
	const size_t startOffset = this->offset;

	//loop
	while (next != '"') {
		if (next == '\\') {
			next = this->getNextChar();
		}
		next = this->getNextChar();
	}

	//save to string
	node.value.len = this->offset - startOffset;
	next = this->getNextChar();
}

/**********************************************************/
void JsonFileCursor::parseNumber(JsonNode & node)
{
	//check
	assert((this->getCurrent() >= '0' && this->getCurrent() <= '9') || this->getCurrent() == '-');

	//register start
	node.type = JSON_NODE_INTEGER;
	node.value.value = this->data->value + this->offset;
	const size_t startOffset = this->offset;

	//loop
	char next = this->getCurrent();
	while ((next >= '0' && next <= '9') || next == '.' || next == 'e' || next == '-' || next == '+') {
		next = this->getNextChar(true);
		if (next == '.' || next == 'e')
			node.type = JSON_NODE_FLOAT;
	}

	//save to string
	node.value.len = this->offset - startOffset;
}

/**********************************************************/
void JsonFileCursor::parseBoolean(JsonNode & node)
{
	if (strncmp("true", this->data->value + this->offset, 4) == 0) {
		node.type = JSON_NODE_BOOL;
		node.value.value = this->data->value + this->offset;
		node.value.len = 4;
	} else if (strncmp("false", this->data->value + this->offset, 5) == 0) {
		node.type = JSON_NODE_BOOL;
		node.value.value = this->data->value + this->offset;
		node.value.len = 5;
	} else {
		this->raiseError("Invalid value for a boolean !");
	}
	for (size_t i = 0 ; i < node.value.len ; i++)
		this->getNextChar();
}

/**********************************************************/
void JsonFileCursor::parseChild(JsonNode & node)
{
	//get it
	JsonCharType cur = this->forwardNextStart();

	//case
	switch(cur) {
		case JSON_CHAR_BOOL_START:
			this->parseBoolean(node);
			break;
		case JSON_CHAR_OBJECT_START:
			this->parseObject(node);
			break;
		case JSON_CHAR_ARRAY_START:
			this->parseArray(node);
			break;
		case JSON_CHAR_STRING_START_END:
			this->parseString(node);
			break;
		case JSON_CHAR_STRING_DIGIT:
			this->parseNumber(node);
			break;
		default:
			this->raiseError("Invalid element starting !");
			break;
	}
}

}
