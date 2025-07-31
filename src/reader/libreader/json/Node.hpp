/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.3.1
*    DATE     : 03/2025
*    LICENSE  : CeCILL-C
*    FILE     : ./src/reader/libreader/json/Node.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
#ifndef MALT_JSON_NODE_HPP
#define MALT_JSON_NODE_HPP

/**********************************************************/
#include <vector>
#include <map>
#include "String.hpp"

/**********************************************************/
namespace MALTJson
{

/**********************************************************/
enum JsonNodeType
{
	JSON_NODE_STRING,
	JSON_NODE_BOOL,
	JSON_NODE_INTEGER,
	JSON_NODE_FLOAT,
	JSON_NODE_OBJECT,
	JSON_NODE_ARRAY,
	JSON_NODE_NONE,
};

/**********************************************************/
struct JsonNode
{
	JsonNode(void) = default;
	JsonNode(JsonNode & orig) = default;
	JsonNode(JsonNode && orig);
	~JsonNode(void);
	JsonNode & operator=(const JsonNode & value) = default;
	JsonNodeType type{JSON_NODE_NONE};
	union {
		std::vector<JsonNode> * vector{nullptr};
		std::map<JsonString, JsonNode> * map;
		std::vector<std::pair<JsonString, JsonNode> > * vmap;
	} view;
	JsonString value;
};

}

#endif //MALT_JSON_NODE_HPP
