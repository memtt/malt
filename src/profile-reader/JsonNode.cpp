/*****************************************************
             PROJECT  : MALT
             VERSION  : 0.3.0
             DATE     : 07/2015
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
//core
#include <common/Debug.hpp>
#include <common/NoFreeAllocator.hpp>
//std
#include <cstring>
#include <cstdio>
//internal
#include "JsonNode.hpp"

/*******************  NAMESPACE  ********************/
namespace MALT
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
		MALT_FATAL("TODO");
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
		MALT_FATAL("Invalid type combination");
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
	char * key = (char*)MALT_NO_FREE_MALLOC(strlen(buffer)+1);
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
				this->content.mapNoFree = MALT_NO_FREE_NEW(JsonNodeMapNoFree);
			else
				this->content.map = new JsonNodeMap;
			break;
		case JSON_NODE_VALUE:
			//nothing to do, wait value
			break;
// 		case JSON_NODE_ARRAY:
// 			if (this->usedStackAllocator)
// 				this->content.arrayNoFree = MALT_NO_FREE_NEW(JsonNodeVectorNoFree);
// 			else
// 				this->content.array = new JsonNodeVector;
// 			break;
		default:
			MALT_FATAL("Unsupported type");
	};
}

/*******************  FUNCTION  *********************/
JsonNode::iterator JsonNode::begin ( void )
{
	MALT_FATAL("TODO");
}

/*******************  FUNCTION  *********************/
JsonNode::iterator JsonNode::end ( void )
{
	MALT_FATAL("TODO");
}

/*******************  FUNCTION  *********************/
bool JsonNode::getAsBool ( bool defaultValue ) const
{
	MALT_FATAL("TODO");
}

/*******************  FUNCTION  *********************/
float JsonNode::getAsFloat ( float defaultValue ) const
{
	MALT_FATAL("TODO");
}

/*******************  FUNCTION  *********************/
int JsonNode::getAsInt ( int defaultValue ) const
{
	MALT_FATAL("TODO");
}

/*******************  FUNCTION  *********************/
long int JsonNode::getAsLong ( long int defaultValue ) const
{
	MALT_FATAL("TODO");
}

/*******************  FUNCTION  *********************/
const char* JsonNode::getAsString ( const char* defaultValue ) const
{
	MALT_FATAL("TODO");
}

/*******************  FUNCTION  *********************/
JsonNodeType JsonNode::getType ( void ) const
{
	return this->type;
}

/*******************  FUNCTION  *********************/
bool JsonNode::isArray ( void ) const
{
	MALT_FATAL("TODO");
}

/*******************  FUNCTION  *********************/
bool JsonNode::isObject ( void ) const
{
	MALT_FATAL("TODO");
}

/*******************  FUNCTION  *********************/
bool JsonNode::isValue ( void ) const
{
	MALT_FATAL("TODO");
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
		MALT_FATAL_ARG("Operator [] cannot be used on none Array or Object, you get %1").arg(type).end();
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
		MALT_FATAL_ARG("Operator [] cannot be used on none Array or Object, you get %1").arg(type).end();
	}
}

/*******************  FUNCTION  *********************/
int JsonNode::size ( void ) const
{
	MALT_FATAL("TODO");
}

/*******************  FUNCTION  *********************/
void JsonNode::unrefChilds ( void )
{
	MALT_FATAL("TODO");
}

/*******************  FUNCTION  *********************/
std::ostream& operator<< ( std::ostream& out, const JsonNodeType& value )
{
	assert(value >= JSON_NODE_NULL && value < JSON_NODE_TYPE_COUNT);
	out << cstJsonNodeTypeNames[value];
	return out;
}

/*******************  FUNCTION  *********************/
JsonNodeIterator::JsonNodeIterator ( JsonNodeIterator& ref )
{
	MALT_FATAL("TODO");
}

/*******************  FUNCTION  *********************/
JsonNodeIterator::JsonNodeIterator ( JsonNode* node )
{
	MALT_FATAL("TODO");
}

/*******************  FUNCTION  *********************/
bool JsonNodeIterator::operator!= ( const JsonNodeIterator& ref ) const
{
	MALT_FATAL("TODO");
}

/*******************  FUNCTION  *********************/
JsonNode& JsonNodeIterator::operator* ( void )
{
	MALT_FATAL("TODO");
}

/*******************  FUNCTION  *********************/
JsonNodeIterator& JsonNodeIterator::operator++ ( int )
{
	MALT_FATAL("TODO");
}

/*******************  FUNCTION  *********************/
JsonNodeIterator& JsonNodeIterator::operator++ ( void )
{
	MALT_FATAL("TODO");
}

/*******************  FUNCTION  *********************/
JsonNode& JsonNodeIterator::operator-> ( void )
{
	MALT_FATAL("TODO");
}

/*******************  FUNCTION  *********************/
JsonNodeIterator& JsonNodeIterator::operator= ( const JsonNodeIterator& ref )
{
	MALT_FATAL("TODO");
}

/*******************  FUNCTION  *********************/
bool JsonNodeIterator::operator== ( const JsonNodeIterator& ref ) const
{
	MALT_FATAL("TODO");
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


}
