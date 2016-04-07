/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.3.0
             DATE     : 07/2015
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
//core
#include <common/Debug.hpp>
#include <allocators/NoFreeAllocator.hpp>
//std
#include <cstring>
#include <cstdio>
//internal
#include "JsonNode.hpp"
#include "../../extern-deps/from-htopml/json/JsonState.h"

/*******************  NAMESPACE  ********************/
namespace MATT
{

/*********************  CONSTS  *********************/
const char * cstJsonNodeTypeNames[] = {
	"JSON_NODE_NULL",
	"JSON_NODE_VALUE",
	"JSON_NODE_ROOT",
	"JSON_NODE_ARRAY",
	"JSON_NODE_OBJECT",
	"JSON_NODE_POINTER"
};

/*******************  FUNCTION  *********************/
JsonNode::JsonNode ( void )
{
	this->initNull();
}

/*******************  FUNCTION  *********************/
JsonNode::JsonNode ( const JsonNode& orig )
{
	if (orig.type == JSON_NODE_NULL) 
	{
		this->initNull();
	} else if (orig.type == JSON_NODE_POINTER) {
		this->initNull();
		this->mutateAs(JSON_NODE_POINTER);
		this->content.pointer = orig.content.pointer;
		this->content.pointer->countRef++;
	} else {
		MATT_FATAL("TODO");
	}
}

/*******************  FUNCTION  *********************/
JsonNode::~JsonNode ( void )
{
	assert(this->countRef == 0);
	if (this->type == JSON_NODE_POINTER)
	{
		this->content.pointer->countRef--;
		assert(this->content.pointer->countRef >= 0);
	} else {
		this->freeContent();
	}
}

/*******************  FUNCTION  *********************/
void JsonNode::freeContent ( void )
{
	if (this->usedStackAllocator)
	{
		this->content.pointer = NULL;
	} else if (this->type == JSON_NODE_ARRAY) {
		delete this->content.map;
	} else if (this->type == JSON_NODE_OBJECT) {
		delete this->content.map;
	} else if (this->type == JSON_NODE_VALUE) {
		delete this->content.value;
	} else if (this->type == JSON_NODE_NULL || this->type == JSON_NODE_ROOT) {
		//nothing to do
	} else {
		MATT_FATAL("Invalid type combination");
	}
}

/*******************  FUNCTION  *********************/
void JsonNode::initNull ( void )
{
	this->countRef = 0;
	this->type = JSON_NODE_NULL;
	this->content.value = NULL;
	this->allocatedContent = false;
	this->usedStackAllocator = false;
}

/*******************  FUNCTION  *********************/
JsonNode& JsonNode::append ( void )
{
	//check
	assume(this->type == JSON_NODE_ARRAY,"Cannot use append() on non array nodes");
	
	//select
	size_t size;
	if (this->usedStackAllocator)
		size = this->content.mapNoFree->size();
	else
		size = this->content.map->size();
	
	//setup
	char buffer[64];
	sprintf(buffer,"%lu",size);
	char * key = (char*)MATT_NO_FREE_MALLOC(strlen(buffer)+1);
	strcpy(key,buffer);
	return (*this)[key];
}

/*******************  FUNCTION  *********************/
void JsonNode::mutateAs(JsonNodeType nodeType)
{
	//nothing to do
	if (this->type == nodeType && this->type == JSON_NODE_VALUE)
		return;
	
	//check
	assume(this->type == JSON_NODE_NULL,"Cannot mutate non NULL json type");
	
	//set
	this->type = nodeType;
	
	//allocate
	switch(nodeType)
	{
		case JSON_NODE_ARRAY:
		case JSON_NODE_OBJECT:
			if (this->usedStackAllocator)
				this->content.mapNoFree = MATT_NO_FREE_NEW(JsonNodeMapNoFree);
			else
				this->content.map = new JsonNodeMap;
			break;
		case JSON_NODE_VALUE:
			//nothing to do, wait value
			break;
// 		case JSON_NODE_ARRAY:
// 			if (this->usedStackAllocator)
// 				this->content.arrayNoFree = MATT_NO_FREE_NEW(JsonNodeVectorNoFree);
// 			else
// 				this->content.array = new JsonNodeVector;
// 			break;
		default:
			MATT_FATAL("Unsupported type");
	};
}

/*******************  FUNCTION  *********************/
JsonNode::iterator JsonNode::begin ( void )
{
	JsonNode::iterator it(this,false);
	return it;
}

/*******************  FUNCTION  *********************/
JsonNode::iterator JsonNode::end ( void )
{
	JsonNode::iterator it(this,true);
	return it;
}

/*******************  FUNCTION  *********************/
bool JsonNode::getAsBool ( bool defaultValue ) const
{
	MATT_FATAL("TODO");
}

/*******************  FUNCTION  *********************/
float JsonNode::getAsFloat ( float defaultValue ) const
{
	MATT_FATAL("TODO");
}

/*******************  FUNCTION  *********************/
int JsonNode::getAsInt ( int defaultValue ) const
{
	MATT_FATAL("TODO");
}

/*******************  FUNCTION  *********************/
long int JsonNode::getAsLong ( long int defaultValue ) const
{
	MATT_FATAL("TODO");
}

/*******************  FUNCTION  *********************/
const char* JsonNode::getAsString ( const char* defaultValue ) const
{
	switch(type)
	{
		case JSON_NODE_NULL:
			return defaultValue;
		case JSON_NODE_VALUE:
			return this->content.value;
		default:
			MATT_FATAL("Invalid use of getAsString on non scalar value");
			break;
	}
}

/*******************  FUNCTION  *********************/
JsonNodeType JsonNode::getType ( void ) const
{
	return this->type;
}

/*******************  FUNCTION  *********************/
bool JsonNode::isArray ( void ) const
{
	MATT_FATAL("TODO");
}

/*******************  FUNCTION  *********************/
bool JsonNode::isObject ( void ) const
{
	MATT_FATAL("TODO");
}

/*******************  FUNCTION  *********************/
bool JsonNode::isValue ( void ) const
{
	MATT_FATAL("TODO");
}

/*******************  FUNCTION  *********************/
const JsonNode& JsonNode::operator[] ( const char* key ) const
{
	if (this->type == JSON_NODE_OBJECT || this->type == JSON_NODE_ARRAY)
	{
		if (this->usedStackAllocator)
		{
			JsonNode & ret = (*this->content.mapNoFree)[key];
			ret.usedStackAllocator = true;
			return ret;
		} else {
			return (*this->content.map)[key];
		}
// 	} else if (this->type == JSON_NODE_ARRAY) {
// 		size_t id = atol(key);
// 		if (this->usedStackAllocator)
// 			return (*this->content.arrayNoFree)[id];
// 		else
// 			return (*this->content.array)[id];
	} else if (this->type == JSON_NODE_POINTER) {
		return (*this->content.pointer)[key];
	} else {
		MATT_FATAL_ARG("Operator [] cannot be used on none Array or Object, you get %1").arg(type).end();
	}
}

/*******************  FUNCTION  *********************/
JsonNode& JsonNode::operator[] ( const char* key )
{
	if (this->type == JSON_NODE_OBJECT || this->type == JSON_NODE_ARRAY)
	{
		if (this->usedStackAllocator)
		{
			JsonNode & ret = (*this->content.mapNoFree)[key];
			ret.usedStackAllocator = true;
			return ret;
		} else {
			return (*this->content.map)[key];
		}
// 	} else if (this->type == JSON_NODE_ARRAY) {
// 		size_t id = atol(key);
// 		if (this->usedStackAllocator)
// 			return (*this->content.arrayNoFree)[id];
// 		else
// 			return (*this->content.array)[id];
	} else {
		MATT_FATAL_ARG("Operator [] cannot be used on none Array or Object, you get %1").arg(type).end();
	}
}

/*******************  FUNCTION  *********************/
int JsonNode::size ( void ) const
{
	MATT_FATAL("TODO");
}

/*******************  FUNCTION  *********************/
void JsonNode::unrefChilds ( void )
{
	MATT_FATAL("TODO");
}

/*******************  FUNCTION  *********************/
void convertToJson(htopml::JsonState& json, const JsonNode& nodeCst)
{
	//TODO clenaup this const cast
	JsonNode & node = (JsonNode &)nodeCst;
	switch (node.type)
	{
		case JSON_NODE_NULL:
			json.openStruct();
			json.closeStruct();
			break;
		case JSON_NODE_VALUE:
			json.printValue(node.getAsString(""));
			break;
		case JSON_NODE_ARRAY:
			json.openArray();
			for (JsonNode::iterator it = node.begin() ; it != node.end() ; ++it)
				json.printValue(*it);
			json.closeArray();
			break;
		case JSON_NODE_OBJECT:
			json.openStruct();
			for (JsonNode::iterator it = node.begin() ; it != node.end() ; ++it)
				json.printField(it.index().c_str(),*it);
			json.closeStruct();
			break;
		default:
			MATT_FATAL("Invalid node type");
	}
// JSON_NODE_NULL,
// 	JSON_NODE_VALUE,
// 	JSON_NODE_ROOT,
// 	JSON_NODE_ARRAY,
// 	JSON_NODE_OBJECT,
// 	JSON_NODE_POINTER,
// 	JSON_NODE_TYPE_COUNT
}

/*******************  FUNCTION  *********************/
std::ostream& operator<< ( std::ostream& out, const JsonNodeType& value )
{
	assert(value >= JSON_NODE_NULL && value < JSON_NODE_TYPE_COUNT);
	out << cstJsonNodeTypeNames[value];
	return out;
}

/*******************  FUNCTION  *********************/
std::ostream& operator<<(std::ostream& out, const JsonNode& node)
{
	htopml::JsonState json(&out);
	convertToJson(json,node);
	return out;
}

/*******************  FUNCTION  *********************/
JsonNodeIterator::JsonNodeIterator ( JsonNode* node, bool end )
{
	this->setup(node,end);
}

/*******************  FUNCTION  *********************/
void JsonNodeIterator::setup(JsonNode* node,bool end)
{
	assert(node != NULL);
	this->node = node;
	switch(node->type)
	{
		case JSON_NODE_NULL:
			MATT_FATAL("Cannot iterate on NULL json node");
			break;
		case JSON_NODE_VALUE:
			MATT_FATAL("cannot iterate on json value");
			break;
		case JSON_NODE_ROOT:
		case JSON_NODE_OBJECT:
			if (node->usedStackAllocator)
				this->mapNfIt = end ? node->content.mapNoFree->end() : node->content.mapNoFree->begin();
			else
				this->mapIt = end ? node->content.map->end() : node->content.map->begin();
			break;
		case JSON_NODE_ARRAY:
			if (node->usedStackAllocator)
				this->vecNfIt = end ? node->content.arrayNoFree->end() : node->content.arrayNoFree->begin();
			else
				this->vecIt = end ? node->content.array->end() : node->content.array->begin();
			break;
		case JSON_NODE_POINTER:
			this->setup(node->content.pointer,end);
			break;
		default:
		case JSON_NODE_TYPE_COUNT:
			MATT_FATAL("Invalid node type");
			break;
	}
}

/*******************  FUNCTION  *********************/
bool JsonNodeIterator::operator!= ( const JsonNodeIterator& ref ) const
{
	//trivial
	if (this->node != ref.node)
		return true;
	
	//detail check
	switch(node->type)
	{
		case JSON_NODE_NULL:
			MATT_FATAL("Cannot iterate on NULL json node");
			break;
		case JSON_NODE_VALUE:
			MATT_FATAL("cannot iterate on json value");
			break;
		case JSON_NODE_ROOT:
		case JSON_NODE_OBJECT:
			if (node->usedStackAllocator)
				return this->mapNfIt != ref.mapNfIt;
			else
				return this->mapIt != ref.mapIt;;
			break;
		case JSON_NODE_ARRAY:
			if (node->usedStackAllocator)
				return this->vecNfIt != ref.vecNfIt;
			else
				return this->vecIt != ref.vecIt;
			break;
		case JSON_NODE_POINTER:
		case JSON_NODE_TYPE_COUNT:
			MATT_FATAL("Invalid node type");
			break;
	}
}

/*******************  FUNCTION  *********************/
JsonNode& JsonNodeIterator::operator* ( void )
{
	//detail check
	switch(node->type)
	{
		case JSON_NODE_NULL:
			MATT_FATAL("Cannot iterate on NULL json node");
			break;
		case JSON_NODE_VALUE:
			MATT_FATAL("cannot iterate on json value");
			break;
		case JSON_NODE_ROOT:
		case JSON_NODE_OBJECT:
			if (node->usedStackAllocator)
				return this->mapNfIt->second;
			else
				return this->mapIt->second;
			break;
		case JSON_NODE_ARRAY:
			if (node->usedStackAllocator)
				return *(this->vecNfIt);
			else
				return *(this->vecIt);
			break;
		case JSON_NODE_POINTER:
		case JSON_NODE_TYPE_COUNT:
			MATT_FATAL("Invalid node type");
			break;
	}
}

/*******************  FUNCTION  *********************/
JsonNodeIterator JsonNodeIterator::operator++ ( int )
{
	JsonNodeIterator it = *this;
	
	//detail check
	switch(node->type)
	{
		case JSON_NODE_NULL:
			MATT_FATAL("Cannot iterate on NULL json node");
			break;
		case JSON_NODE_VALUE:
			MATT_FATAL("cannot iterate on json value");
			break;
		case JSON_NODE_ROOT:
		case JSON_NODE_OBJECT:
			if (node->usedStackAllocator)
				++(it.mapNfIt);
			else
				++(it.mapIt);
			break;
		case JSON_NODE_ARRAY:
			if (node->usedStackAllocator)
				++(it.vecNfIt);
			else
				++(it.vecIt);
			break;
		case JSON_NODE_POINTER:
		case JSON_NODE_TYPE_COUNT:
			MATT_FATAL("Invalid node type");
			break;
	}
	return it;
}

/*******************  FUNCTION  *********************/
JsonNodeIterator& JsonNodeIterator::operator++ ( void )
{
	//detail check
	switch(node->type)
	{
		case JSON_NODE_NULL:
			MATT_FATAL("Cannot iterate on NULL json node");
			break;
		case JSON_NODE_VALUE:
			MATT_FATAL("cannot iterate on json value");
			break;
		case JSON_NODE_ROOT:
		case JSON_NODE_OBJECT:
			if (node->usedStackAllocator)
				++(this->mapNfIt);
			else
				++(this->mapIt);
			break;
		case JSON_NODE_ARRAY:
			if (node->usedStackAllocator)
				++(this->vecNfIt);
			else
				++(this->vecIt);
			break;
		case JSON_NODE_POINTER:
		case JSON_NODE_TYPE_COUNT:
			MATT_FATAL("Invalid node type");
			break;
	}
	return *this;
}

/*******************  FUNCTION  *********************/
const std::string JsonNodeIterator::index(void)
{
	//detail check
	switch(node->type)
	{
		case JSON_NODE_NULL:
			MATT_FATAL("Cannot iterate on NULL json node");
			break;
		case JSON_NODE_VALUE:
			MATT_FATAL("cannot iterate on json value");
			break;
		case JSON_NODE_ROOT:
		case JSON_NODE_OBJECT:
			if (node->usedStackAllocator)
				return (this->mapNfIt->first.value);
			else
				return (this->mapIt->first);
			break;
		case JSON_NODE_ARRAY:
			MATT_FATAL("Not supported on array");
			break;
		case JSON_NODE_POINTER:
		case JSON_NODE_TYPE_COUNT:
			MATT_FATAL("Invalid node type");
			break;
	}
}

/*******************  FUNCTION  *********************/
JsonNode& JsonNodeIterator::operator-> ( void )
{
	return **this;
}

/*******************  FUNCTION  *********************/
JsonNodeIterator& JsonNodeIterator::operator= ( const JsonNodeIterator& ref )
{
	*this = ref;
}

/*******************  FUNCTION  *********************/
bool JsonNodeIterator::operator== ( const JsonNodeIterator& ref ) const
{
	//trivial
	if (this->node != ref.node)
		return false;
	
	//detail check
	switch(node->type)
	{
		case JSON_NODE_NULL:
			MATT_FATAL("Cannot iterate on NULL json node");
			break;
		case JSON_NODE_VALUE:
			MATT_FATAL("cannot iterate on json value");
			break;
		case JSON_NODE_ROOT:
		case JSON_NODE_OBJECT:
			if (node->usedStackAllocator)
				return this->mapNfIt == ref.mapNfIt;
			else
				return this->mapIt == ref.mapIt;;
			break;
		case JSON_NODE_ARRAY:
			if (node->usedStackAllocator)
				return this->vecNfIt == ref.vecNfIt;
			else
				return this->vecIt == ref.vecIt;
			break;
		case JSON_NODE_POINTER:
		case JSON_NODE_TYPE_COUNT:
			MATT_FATAL("Invalid node type");
			break;
	}
}

/*******************  FUNCTION  *********************/
StringPtr::StringPtr ( void )
{
	this->value = "";
}

/*******************  FUNCTION  *********************/
StringPtr::StringPtr ( const char* value )
{
	this->value = value;
}

/*******************  FUNCTION  *********************/
bool operator< ( const StringPtr& a, const StringPtr& b )
{
	return strcmp(a.value,b.value) < 0;
}

/*******************  FUNCTION  *********************/
JsonNode& JsonNode::operator= ( const JsonNode& orig )
{
	assume(orig.type == JSON_NODE_NULL,"Can only affect null objects");
	this->initNull();
}

/*******************  FUNCTION  *********************/
JsonNode& JsonNode::getChild(const std::string& path)
{
	//vars
	char * tmp = new char[path.size()+1];
	strcpy(tmp,path.c_str());
	char * token = strtok(tmp,".");
	JsonNode * node = this;
	
	//loop on all
	while (token != NULL)
	{
		if (*token != '\0')
		{
			node = &(*node)[token];
			token = strtok(NULL,".");
		}
	}
	
	return *node;
}

}
