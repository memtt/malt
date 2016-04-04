/*****************************************************
             PROJECT  : MALT
             VERSION  : 0.3.0
             DATE     : 07/2015
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
//internals
#include "JsonNodeLoader.hpp"
//from core
#include "common/Debug.hpp"
//std
#include <cstdio>
#include <cstring>
//unix
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


/*******************  NAMESPACE  ********************/
namespace MALT
{

/*******************  FUNCTION  *********************/
void JsonNodeLoader::loadFromData ( char* raw, bool usedStackAllocator )
{
	//errors
	assert(raw != NULL);
	this->usedStackAllocator = usedStackAllocator;
	
	//setup state
	JsonLoaderState state(raw);
	
	//start to load
	loadFromData(state,NULL);
	
	//check root types
	assumeArg(this->getType() == JSON_NODE_OBJECT,"Root object must be an object, get %1").arg(this->getType()).end();
	
	//check if we reach end of file
	state.skipWhite();
	assume(*state == '\0',"Failed to read the content until end of file, maybe there is an error in your file !");
}

/*******************  FUNCTION  *********************/
void JsonNodeLoader::loadFromData ( JsonLoaderState& state, JsonNodeLoader * parent )
{
	//save start state
	JsonLoaderState startState = state;
	
	//skip white
	state.skipWhite();
	
	//check for root
	if (parent == NULL)
		assume(*state == '{',"Root node must be an object starting with '{' !");
	
	//check type
	switch(*state)
	{
		case '{':
			this->loadFromDataAsObject(state,parent);
			break;
		case '[':
			//load as array
			this->loadFromDataAsArray(state,parent);
			break;
		case ',':
			//null object nothing to do
			break;
		case '}':
			//mistake
			MALT_FATAL_ARG("Get unexpecte '}' at position %1").arg(state).end();
			break;
		case ']':
			//mistake
			MALT_FATAL_ARG("Get unexpecte ']' at position %1").arg(state).end();
			break;
		case '"':
		case '\'':
			//load as protected string value
			this->loadFromDataAsString(state,parent);
			break;
		case 't':
		case 'f':
			//load as boolean
			this->loadFromDataAsBool(state,parent);
			break;
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
			//load as number
			this->loadFromDataAsNumber(state,parent);
			break;
		default:
			//load as string value
			MALT_FATAL("TODO");
			break;
	}
}

/*******************  FUNCTION  *********************/
void JsonNodeLoader::loadFromDataAsBool ( JsonLoaderState& state, JsonNodeLoader* parent )
{
	//keep start
	JsonLoaderState start = state;
	char * res = start.get();
	
	if (strncmp(res,"true",4) == 0)
	{
		for(int i = 0 ; i < 4 ; i++)
			++state;
	} else if (strncmp(res,"false",5) == 0) {
		for(int i = 0 ; i < 5 ; i++)
			++state;
	} else {
		for(int i = 0 ; i < 10 ; i++)
			++state;
		state.markEnd();
		MALT_FATAL_ARG("Expect true of false at %1, but get %2").arg(start).arg(start.get()).end();
	}
	
	//check end
	state.isEndOfValue();
	
	//mark end
	state.markEnd();
	
	//setup
	this->mutateAs(JSON_NODE_VALUE);
	this->content.value = res;
}

/*******************  FUNCTION  *********************/
void JsonNodeLoader::loadFromDataAsString ( JsonLoaderState& state, JsonNodeLoader* parent )
{
	char * value = this->loadStringFromData(state);
	this->mutateAs(JSON_NODE_VALUE);
	this->content.value = value;
}

/*******************  FUNCTION  *********************/
void JsonLoaderState::skipWhite ( void )
{
	while (**this == ' ' || **this == '\t' || **this == '\n')
		++(*this);
}

/*******************  FUNCTION  *********************/
void JsonNodeLoader::loadFromDataAsNumber ( JsonLoaderState& state, JsonNodeLoader* parent )
{
	//keep start
	JsonLoaderState start = state;
	char * res = start.get();
	
	//read all
	while ((*state >= '0' && *state <= '9') || *state == '.')
		++state;
	
	//check end
	state.isEndOfValue();
	
	//replace
	state.markEnd();
	
	//setup
	this->mutateAs(JSON_NODE_VALUE);
	this->content.value = res;
}

/*******************  FUNCTION  *********************/
void JsonNodeLoader::loadFromDataAsArray ( JsonLoaderState& state, JsonNodeLoader* parent )
{
	//check
	assert(*state == '[');
	
	//make it array
	this->mutateAs(JSON_NODE_ARRAY);
	
	//move next & skip white space
	++state;
	state.skipWhite();
	
	//load childs
	while (*state != ']') 
	{
		//create object
		JsonNodeLoader &child = static_cast<JsonNodeLoader&>(this->append());
		
		//load it
		child.loadFromData(state,this);
		
		//move and search ',' || '}'
		state.skipWhite();
		assumeArg(*state == ',' || *state == ']',"Expect separator between object (,) or end of object (]) at %1, get %2")
			.arg(state)
			.arg(*state)
			.end();
		
		//move to next object if has one
		if (*state == ',')
		{
			++state;
			state.skipWhite();
		}
	}
	
	//move next
	++state;
}

/*******************  FUNCTION  *********************/
void JsonNodeLoader::loadFromDataAsObject ( JsonLoaderState& state, JsonNodeLoader* parent )
{
	//check
	assert(*state == '{');
	
	//make it object
	this->mutateAs(JSON_NODE_OBJECT);
	
	//move next & skip white space
	++state;
	state.skipWhite();
	
	//load childs
	while (*state != '}') 
	{
		//extract key
		char * key = loadStringFromData(state);
		
		//skip
		state.skipWhite();
		
		//check separator
		assumeArg(*state == ':',"Expect separator between key and value (:) at %1, get %2")
			.arg(state)
			.arg(*state)
			.end();
		++state;
		
		//create object
		JsonNodeLoader &child = static_cast<JsonNodeLoader&>((*this)[key]);
		
		//load it
		child.loadFromData(state,this);
		
		//move and search ',' || '}'
		state.skipWhite();
		assumeArg(*state == ',' || *state == '}',"Expect separator between object (,) or end of object (}) at %1, get %2")
			.arg(state)
			.arg(*state)
			.end();
		
		//move to next object if has one
		if (*state == ',')
		{
			++state;
			state.skipWhite();
		}
	}
	
	//move next
	++state;
}

/*******************  FUNCTION  *********************/
char* JsonNodeLoader::loadStringFromData ( JsonLoaderState& state )
{
	//check
	assumeArg(*state == '"' || *state == '\'',"At %1, expect string stargint with quote (' or \") but get %2")
		.arg(state)
		.arg(*state)
		.end();
	
	//remember close
	JsonLoaderState start = state;
	char close = *state;
	
	//move forward
	++state;
	char * res = state.get();
	
	//read unit close
	while (*state != close)
	{
		if (*state == '\\')
			++state;
		assumeArg(*state != '\0',"Reach end of file before finishing reading string started at %1")
			.arg(start)
			.end();
		++state;
	}
	
	//mark end
	state.markEnd();
	
	//move to next
	++state;
	
	return res;
}

/*******************  FUNCTION  *********************/
char JsonLoaderState::operator* ( void )
{
	if (replaced != '\0')
		return replaced;
	else
		return *cur;
}

/*******************  FUNCTION  *********************/
JsonLoaderState& JsonLoaderState::operator++ ( void )
{
	if (*cur == '\n')
	{
		pos = 0;
		line++;
	} else {
		pos++;
	}
	cur++;
	replaced = '\0';
	return *this;
}

/*******************  FUNCTION  *********************/
std::ostream& operator<< ( std::ostream& out, const JsonLoaderState& state )
{
	out << "position " << state.pos << " and line " << state.line;
	return out;
}

/*******************  FUNCTION  *********************/
void JsonLoaderState::isEndOfValue ( void )
{
	assumeArg(**this == ' ' || **this == '\t' || **this == '\n' || **this == ',' || **this == ']' || **this == '}',
			  "Get invalid end of value, expect white space or (',' | '}' | ']') but get %1 at %2")
			.arg(**this)
			.arg(*this)
			.end();
}

/*******************  FUNCTION  *********************/
JsonLoaderState::JsonLoaderState ( char* value )
{
	this->pos = 0;
	this->line = 0;
	this->cur = value;
	this->replaced = '\0';
}

/*******************  FUNCTION  *********************/
void JsonLoaderState::replace ( char value )
{
	this->replaced = *cur;
	*cur = value;
}

/*******************  FUNCTION  *********************/
char* JsonLoaderState::get ( void )
{
	assert(replaced == '\0');
	return cur;
}

/*******************  FUNCTION  *********************/
void JsonLoaderState::markEnd ( void )
{
	this->replace('\0');
}

}
