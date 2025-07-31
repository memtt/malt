#ifndef MALT_JSON_PARSER_HPP
#define MALT_JSON_PARSER_HPP

#include <list>
#include <cassert>
#include "Exception.hpp"
#include "String.hpp"
#include "Node.hpp"
#include "../loader/ProgressBar.hpp"

namespace MALTJson
{

enum JsonCharType
{
	JSON_CHAR_SEPARATOR,
	JSON_CHAR_OBJECT_START,
	JSON_CHAR_OBJECT_END,
	JSON_CHAR_ARRAY_START,
	JSON_CHAR_ARRAY_END,
	JSON_CHAR_STRING_START_END,
	JSON_CHAR_BOOL_START,
	JSON_CHAR_STRING_DIGIT,
	JSON_CHAR_FIELD_SEP,
	JSON_CHAR_ERROR,
};

struct JsonFileCursor
{
	const std::string * fname{nullptr};
	const JsonString * data{nullptr};
	MALTReader::ProgressBar * progressBar{nullptr};
	size_t line{0};
	size_t offset{0};
	size_t column{0};
	JsonCharType forwardNextStart(void);
	inline void forwardWhite(void);
	inline char getNextChar(bool forward = true);
	inline char getCurrent(void) const;
	void raiseError(const std::string & message) const;
	inline bool isWhite(char value) const;
	void parseObject(JsonNode & node);
	void parseArray(JsonNode & node);
	void parseString(JsonNode & node);
	void parseNumber(JsonNode & node);
	void parseBoolean(JsonNode & node);
	void parseChild(JsonNode & node);
	void pushObjectChilds(JsonNode & node, std::list<std::pair<JsonString, JsonNode> > & childs);
	void pushObjectChilds(JsonNode & node, std::vector<std::pair<JsonString, JsonNode> > & childs);
};

char JsonFileCursor::getNextChar(bool forward)
{
	//error
	if (this->offset == this->data->len - 1)
		this->raiseError("Unexpected end of file !");

	//direct
	if (forward == false)
		return this->data->value[this->offset + 1];

	//newline
	if (this->data->value[this->offset++] == '\n') {
		this->column = 0;
		this->line++;
	} else {
		this->column++;
	}

	//progress bar
	if (this->progressBar != nullptr) {
		this->progressBar->progress(1);
	}

	//ok
	return this->data->value[this->offset];
}

char JsonFileCursor::getCurrent(void) const
{
	assert(this->offset < this->data->len);
	return this->data->value[this->offset];
}

bool JsonFileCursor::isWhite(char value) const
{
	return value == '\t' || value == '\n' || value == ' ';
}

void JsonFileCursor::forwardWhite(void)
{
	while(this->isWhite(this->getCurrent())) {
		this->getNextChar();
	};
}

}

#endif //MALT_JSON_PARSER_HPP
