/*****************************************************
             PROJECT  : MALT
             VERSION  : 0.3.0
             DATE     : 07/2015
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MALT_JSON_READER_HPP
#define MALT_JSON_READER_HPP

/********************  HEADERS  *********************/
//std
#include <string>
//internal
#include "JsonNode.hpp"

/*******************  NAMESPACE  ********************/
namespace MALT
{

/*********************  STRUCT  *********************/
struct JsonLoaderState
{
	char * cur;
	int line;
	int pos;
};

/*********************  CLASS  **********************/
class JsonNodeLoader : public JsonNode
{
	public:
		void loadFromData(char * raw,bool usedStackAllocator);
		void loadFromData( MALT::JsonLoaderState& state, MALT::JsonNodeLoader* parent );
		void skipWhite(JsonLoaderState & state);
};

/*********************  CLASS  **********************/
class JsonReader
{
	public:
		JsonReader(void);
		~JsonReader(void);
		void loadString(const char * value);
		void load(const std::string & filename);
		JsonNode & getRoot(void);
	protected:
		void parse(void);
		void ungzip(void);
		size_t getFileSize(int fd);
	protected:
		char * raw;
		JsonNodeLoader root;
};

}

#endif //MALT_JSON_READER_HPP
