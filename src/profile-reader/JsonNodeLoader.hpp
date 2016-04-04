/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.3.0
             DATE     : 07/2015
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MATT_JSON_NODE_LOADER_HPP
#define MATT_JSON_NODE_LOADER_HPP

/********************  HEADERS  *********************/
//std
#include <string>
//internal
#include "JsonNode.hpp"

/*******************  NAMESPACE  ********************/
namespace MATT
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
		void loadFromData( MATT::JsonLoaderState& state, MATT::JsonNodeLoader* parent );
		void loadFromDataAsObject(MATT::JsonLoaderState& state, MATT::JsonNodeLoader* parent );
		void loadFromDataAsNumber(MATT::JsonLoaderState& state, MATT::JsonNodeLoader* parent );
		void loadFromDataAsString(MATT::JsonLoaderState& state, MATT::JsonNodeLoader* parent );
		void loadFromDataAsBool(MATT::JsonLoaderState& state, MATT::JsonNodeLoader* parent );
		void loadFromDataAsArray(MATT::JsonLoaderState& state, MATT::JsonNodeLoader* parent );
		char * loadStringFromData(MATT::JsonLoaderState& state);
};

/*******************  FUNCTION  *********************/
std::ostream & operator << (std::ostream& out,const JsonLoaderState & state);

}

#endif //MATT_JSON_NODE_LOADER_HPP
