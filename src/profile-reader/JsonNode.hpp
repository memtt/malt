/*****************************************************
             PROJECT  : MALT
             VERSION  : 0.3.0
             DATE     : 07/2015
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MALT_JSON_NODE_HPP
#define MALT_JSON_NODE_HPP

/********************  HEADERS  *********************/
//std
#include <map>
#include <string>
#include <vector>
#include <ostream>
//from malt core
#include "../lib/common/STLInternalAllocatorNoFree.hpp"

/*******************  NAMESPACE  ********************/
namespace MALT
{

/********************  ENUM  ************************/
enum JsonNodeType
{
	JSON_NODE_NULL,
	JSON_NODE_VALUE,
	JSON_NODE_ROOT,
	JSON_NODE_ARRAY,
	JSON_NODE_OBJECT,
	JSON_NODE_POINTER,
	JSON_NODE_TYPE_COUNT
};

/*********************  STRUCT  *********************/
struct StringPtr
{
	StringPtr(void);
	StringPtr(const char * value);
	const char * value;
};

/*********************  TYPES  **********************/
class JsonNode;
typedef std::map<std::string,JsonNode> JsonNodeMap;
typedef std::vector<JsonNode> JsonNodeVector;
typedef std::map<StringPtr,JsonNode,std::less<StringPtr>,MALTV2::STLInternalAllocatorNoFree< JsonNode > > JsonNodeMapNoFree;
typedef std::vector<JsonNode,MALTV2::STLInternalAllocatorNoFree< JsonNode > > JsonNodeVectorNoFree;

/*********************  CLASS  **********************/
class JsonNodeIterator
{
	public:
		JsonNodeIterator(JsonNode * node);
		JsonNodeIterator(JsonNodeIterator & ref);
		JsonNode & operator*(void);
		JsonNode & operator->(void);
		JsonNodeIterator & operator++(void);
		JsonNodeIterator & operator++(int);
		bool operator == (const JsonNodeIterator & ref) const;
		bool operator != (const JsonNodeIterator & ref) const;
		JsonNodeIterator & operator = (const JsonNodeIterator & ref);
	private:
		JsonNode * node;
		JsonNodeMap::iterator mapIt;
		JsonNodeVector::iterator vecIt;
		JsonNodeMapNoFree::iterator mapNfIt;
		JsonNodeVectorNoFree::iterator vecNfIt;
		
};

/*********************  TYPES  **********************/
union JsonContent
{
	JsonNodeMap * map;
	JsonNodeVector * array;
	JsonNodeMapNoFree * mapNoFree;
	JsonNodeVectorNoFree * arrayNoFree;
	JsonNode * pointer;
	char * value;
};

/*********************  CLASS  **********************/
class JsonNode
{
	public:
		typedef JsonNodeIterator iterator;
		friend JsonNodeIterator;
	public:
		JsonNode(void);
		JsonNode(const JsonNode & orig);
		~JsonNode(void);
		const char * getAsString(const char * defaultValue = "") const;
		int getAsInt(int defaultValue) const;
		float getAsFloat(float defaultValue) const;
		long getAsLong(long defaultValue) const;
		bool getAsBool(bool defaultValue) const;
		JsonNodeType getType(void) const;
		bool isArray(void) const;
		bool isObject(void) const;
		bool isValue(void) const;
		int size(void) const;
		JsonNode & operator[] (const char * key);
		const JsonNode & operator[] (const char * key) const;
		JsonNode & append(void);
		//iterators
		iterator begin(void);
		iterator end(void);
		JsonNode & operator=(const JsonNode & orig);
	protected:
		void unrefChilds(void);
		void mutateAs(JsonNodeType nodeType);
		void initNull(void);
		void freeContent(void);
	protected:
		JsonContent content;
		int countRef;
		JsonNodeType type;
		bool allocatedContent;
		bool usedStackAllocator;
};

/*******************  FUNCTION  *********************/
std::ostream & operator << (std::ostream & out, const JsonNodeType & value);
bool operator < (const StringPtr & a,const StringPtr & b);

}

#endif //MALT_JSON_NODE_HPP
