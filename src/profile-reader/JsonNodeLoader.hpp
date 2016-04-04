/*****************************************************
             PROJECT  : MALT
             VERSION  : 0.3.0
             DATE     : 07/2015
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MALT_JSON_NODE_LOADER_HPP
#define MALT_JSON_NODE_LOADER_HPP

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
	JsonLoaderState(char * value);
	JsonLoaderState & operator++(void);
	char operator*(void);
	void skipWhite(void);
	void isEndOfValue(void);
	void replace(char value);
	void markEnd(void);
	char * get(void);
	char * cur;
	char replaced;
	int line;
	int pos;
};

/*********************  CLASS  **********************/
class JsonNodeLoader : public JsonNode
{
	public:
		void loadFromData(char * raw,bool usedStackAllocator);
	protected:
		void loadFromData( MALT::JsonLoaderState& state, MALT::JsonNodeLoader* parent );
		void loadFromDataAsObject(MALT::JsonLoaderState& state, MALT::JsonNodeLoader* parent );
		void loadFromDataAsNumber(MALT::JsonLoaderState& state, MALT::JsonNodeLoader* parent );
		void loadFromDataAsString(MALT::JsonLoaderState& state, MALT::JsonNodeLoader* parent );
		void loadFromDataAsBool(MALT::JsonLoaderState& state, MALT::JsonNodeLoader* parent );
		void loadFromDataAsArray(MALT::JsonLoaderState& state, MALT::JsonNodeLoader* parent );
		char * loadStringFromData(MALT::JsonLoaderState& state);
};

/*******************  FUNCTION  *********************/
std::ostream & operator << (std::ostream& out,const JsonLoaderState & state);

}

#endif //MALT_JSON_NODE_LOADER_HPP
