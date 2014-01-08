/*****************************************************
             PROJECT  : htopml
             VERSION  : 0.1.0-dev
             DATE     : 11/2012
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
#include <cstdarg>
#include <cassert>
#include <cstdio>
#include "JsonState.h"

/**********************  USING  *********************/
using namespace std;

//Caution we use \n for better performances, on iostream std::endl will flush io for each line.
#define LINE_BREAK "\n"

/********************  NAMESPACE  *******************/
namespace htopml
{

/*******************  FUNCTION  *********************/
JsonState::JsonState(std::ostream* out)
{
	this->out = out;
	this->indent = 0;
	pushState(JSON_STATE_ROOT);
}

/*******************  FUNCTION  *********************/
void JsonState::putPadding()
{
	for (int i = 0 ; i < indent ; i++)
		*out << '\t';
}

/*******************  FUNCTION  *********************/
JsonStateEnum JsonState::getState(void ) const
{
	return stateStack.top().state;
}

/*******************  FUNCTION  *********************/
void JsonState::printFormattedField(const char * name, const char* format, ... )
{
	char buffer[1024];

	//format the chain
	va_list param;
	va_start (param, format);
	vsnprintf (buffer,sizeof(buffer), format, param);
	va_end (param);

	//print
	openField(name);
	*out << buffer;
	closeField(name);
}

/*******************  FUNCTION  *********************/
void JsonState::printFormattedValue(const char* format, ... )
{
	char buffer[1024];

	//check where we are
	assert(getState() & (JSON_STATE_FIELD | JSON_STATE_ARRAY));

	//separator
	if (getState() == JSON_STATE_ARRAY && !isFirst())
		*out << ", ";

	//format the chain
	va_list param;
	va_start (param, format);
	vsnprintf (buffer,sizeof(buffer), format, param);
	va_end (param);

	//print
	*out << buffer;
	firstIsDone();
}

/*******************  FUNCTION  *********************/
void JsonState::openField(const string& name)
{
	//check where we are
	assert(getState() & (JSON_STATE_ROOT | JSON_STATE_STRUCT));

	//print name
	if (!isFirst())
		*out << "," << LINE_BREAK;

	//setup state
	pushState(JSON_STATE_FIELD);

	//print padding
	putPadding();

	//print name
	*out << "\"" << name << "\"" << ": ";
}

/*******************  FUNCTION  *********************/
void JsonState::closeField(const string& name)
{
	//check where we are
	assert(getState() == JSON_STATE_FIELD);

	//setup state
	popState(JSON_STATE_FIELD);

	//mark first as done
	firstIsDone();
}

/*******************  FUNCTION  *********************/
void JsonState::openFieldArray(const char* name)
{
	openField(name);
	openArray();
}

/*******************  FUNCTION  *********************/
void JsonState::closeFieldArray(const char * name)
{
	closeArray();
	closeField(name);
}


/*******************  FUNCTION  *********************/
void JsonState::openArray(void)
{
	//check where we are
	assert(getState() & (JSON_STATE_ROOT | JSON_STATE_FIELD));

	//setup state
	pushState(JSON_STATE_ARRAY);

	//print name
	*out << "[ ";
}

/*******************  FUNCTION  *********************/
void JsonState::closeArray(void)
{
	//check where we are
	assert(getState() == JSON_STATE_ARRAY);

	//setup state
	popState(JSON_STATE_ARRAY);

	//print name
	*out << "]";
}

/*******************  FUNCTION  *********************/
void JsonState::openStruct(void )
{
		//check where we are
	assert(getState() & (JSON_STATE_FIELD | JSON_STATE_ARRAY | JSON_STATE_ROOT));

	//setup state
	pushState(JSON_STATE_STRUCT);

	//print name
	indent++;
	*out << "{" << LINE_BREAK;
}

/*******************  FUNCTION  *********************/
void JsonState::closeStruct(void )
{
	//check where we are
	assert(getState() == JSON_STATE_STRUCT);

	//setup state
	popState(JSON_STATE_STRUCT);

	//print name
	indent--;
	*out << LINE_BREAK;
	putPadding();
	*out << "}";
}

/*******************  FUNCTION  *********************/
void JsonState::firstIsDone(void )
{
	stateStack.top().isFirst = false;
}

/*******************  FUNCTION  *********************/
bool JsonState::isFirst(void ) const
{
	return stateStack.top().isFirst;
}

/*******************  FUNCTION  *********************/
void JsonState::pushState(JsonStateEnum state)
{
	JsonStateStruct tmp = {state,true};
	stateStack.push(tmp);
}

/*******************  FUNCTION  *********************/
void JsonState::popState(JsonStateEnum state)
{
	assert(stateStack.top().state == state);
	stateStack.pop();
}

}
