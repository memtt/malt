/*****************************************************
             PROJECT  : htopml
             VERSION  : 0.1.0-dev
             DATE     : 11/2012
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/**********************  INFO  **********************/
/* Imported from htopml project under CeCILL-C licence */

/********************  HEADERS  *********************/
//extern standard headers
#include <cstdio>
#include <cstdarg>
#include <cassert>
//internal headers
#include "JsonState.h"

/**********************  USING  *********************/
using namespace std;

/**
 * We use \n for better performances, on iostream std::endl will flush io 
 * for each line.
**/
#define LINE_BREAK "\n"
/**
 * Size of buffers to convers values from sprintf formats.
 * Caution, buffers are allocated into the stack, dont use to large values here.
**/
#define SPRINTF_BUFFER_SIZE 1024

/********************  NAMESPACE  *******************/
namespace htopml
{

/*******************  FUNCTION  *********************/
/**
 * Constructor of the JSonState class. It setup the state as JSON_STATE_ROOT
 * and init indent to 0.
 * @param out Define the output stream into which to write json output. NULL isn't supported here.
 * @param useIndent If true, indent the output json code with tabulations, produce 
 * compact json code.
**/
JsonState::JsonState(ostream* out, bool useIndent, bool lua)
{
	//errors
	assert(out != NULL);
	
	//setup
	this->lua = lua;
	this->out = out;
	this->indent = 0;
	this->useIndent = useIndent;
	pushState(JSON_STATE_ROOT);
}

/*******************  FUNCTION  *********************/
/**
 * Write padding characters into output stream. It will use the local indent
 * parameter and use tabulations.
**/
void JsonState::putPadding()
{
	if (useIndent)
	{
		if (indent < 256)
		{
			//faster buffered version
			char buffer[256];
			assert(indent < 256);
			for (int i = 0 ; i < indent ; i++)
				buffer[i] = '\t';
			buffer[indent] = '\0';
			*out << buffer;
		} else {
			//slow unbuffered version
			for (int i = 0 ; i < indent ; i++)
				*out << '\t';
		}
	}
}

/*******************  FUNCTION  *********************/
/**
 * @return Return the current state for checking.
**/
JsonStateEnum JsonState::getState(void ) const
{
	return stateStack.top().state;
}

/*******************  FUNCTION  *********************/
/**
 * Use printf syntaxe to format new field.
 * @param name Name of the field to print.
 * @param format Define the format to use in internal sprint method.
**/
void JsonState::printFormattedField(const char * name, const char* format, ... )
{
	char buffer[SPRINTF_BUFFER_SIZE];

	//format the chain
	va_list param;
	va_start (param, format);
	size_t size = vsnprintf (buffer,sizeof(buffer), format, param);
	va_end (param);
	
	//check buffer overflow
	assert(size <= SPRINTF_BUFFER_SIZE);

	//print
	openField(name);
	*out << buffer;
	closeField(name);
}

/*******************  FUNCTION  *********************/
/**
 * Use printf syntaxe to format new single value. This functions aims to be used to fill
 * arrays.
 * @param format Define the format to use in internal sprintf method.
**/
void JsonState::printFormattedValue(const char* format, ... )
{
	char buffer[SPRINTF_BUFFER_SIZE];

	//check where we are
	assert(getState() & (JSON_STATE_FIELD | JSON_STATE_ARRAY));

	//separator
	if (getState() == JSON_STATE_ARRAY && !isFirst())
		*out << ", ";

	//format the chain
	va_list param;
	va_start (param, format);
	size_t size = vsnprintf (buffer,sizeof(buffer), format, param);
	va_end (param);
	
	//check buffer overflow
	assert(size <= SPRINTF_BUFFER_SIZE);

	//print
	*out << buffer;
	firstIsDone();
}

/*******************  FUNCTION  *********************/
/**
 * Internal function to start a new field.
 * @param name Name of the field to declare.
**/
void JsonState::openField(const char * name)
{
	//check where we are
	assert(getState() & (JSON_STATE_ROOT | JSON_STATE_STRUCT));

	//print name
	if (!isFirst())
	{
		if (useIndent)
			*out << "," LINE_BREAK;
		else
			*out << ",";
	}

	//setup state
	pushState(JSON_STATE_FIELD);

	//print padding
	putPadding();

	//print name
	if (lua)
		*out << name << "=";
	else
		*out << "\"" << name << "\"" << ":";
}

/*******************  FUNCTION  *********************/
/**
 * Internal function to close the current field.
 * @param name Name of the field to close (for checking).
**/
void JsonState::closeField(const char * name)
{
	//check where we are
	assert(getState() == JSON_STATE_FIELD);

	//setup state
	popState(JSON_STATE_FIELD);

	//mark first as done
	firstIsDone();
}

/*******************  FUNCTION  *********************/
/**
 * Start a new field with array as content. Internal values must be declared
 * with printFormattedValue() or printValue().
 * It must be closed by closeFieldArray().
 * @param name Name of the field to declare.
**/
void JsonState::openFieldArray(const char* name)
{
	openField(name);
	openArray();
}

/*******************  FUNCTION  *********************/
/**
 * Close a field array opened by openFieldArray().
 * @param name Define the field to terminate (only for checking).
**/
void JsonState::closeFieldArray(const char * name)
{
	closeArray();
	closeField(name);
}


/*******************  FUNCTION  *********************/
/**
 * Start a new array, mostly to be used internally or for the root element.
**/
void JsonState::openArray(void)
{
	//check where we are
	assert(getState() & (JSON_STATE_ROOT | JSON_STATE_FIELD));

	//setup state
	pushState(JSON_STATE_ARRAY);

	//print name
	if (lua)
		*out << "{";
	else
		*out << "[";
}

/*******************  FUNCTION  *********************/
/**
 * Close the current array. Mostly to be used internally or for the root element.
**/
void JsonState::closeArray(void)
{
	//check where we are
	assert(getState() == JSON_STATE_ARRAY);

	//setup state
	popState(JSON_STATE_ARRAY);

	//print name
	if (lua)
		*out << "}";
	else
		*out << "]";
}

/*******************  FUNCTION  *********************/
/**
 * Start a new structure. To be used internally, for root elements or for values inside arrays.
**/
void JsonState::openStruct(void )
{
		//check where we are
	assert(getState() & (JSON_STATE_FIELD | JSON_STATE_ARRAY | JSON_STATE_ROOT));

	//setup state
	pushState(JSON_STATE_STRUCT);

	//print name
	indent++;
	*out << "{";
	if (useIndent)
		*out << LINE_BREAK;
}

/*******************  FUNCTION  *********************/
/**
 * Close the current structure. To be used internally, for root elements or for values inside arrays.
**/
void JsonState::closeStruct(void )
{
	//check where we are
	assert(getState() == JSON_STATE_STRUCT);

	//setup state
	popState(JSON_STATE_STRUCT);

	//print name
	indent--;
	if (useIndent)
		*out << LINE_BREAK;
	putPadding();
	*out << "}";
}

/*******************  FUNCTION  *********************/
/**
 * Check if a first element was provided in case of arrays or structure.
 * Usefull to know if we need to add separators or not.
**/
void JsonState::firstIsDone(void )
{
	stateStack.top().isFirst = false;
}

/*******************  FUNCTION  *********************/
/**
 * Check if a first element was provided in case of arrays or structure.
 * Usefull to know if we need to add separators or not.
**/
bool JsonState::isFirst(void ) const
{
	return stateStack.top().isFirst;
}

/*******************  FUNCTION  *********************/
/**
 * Push the new state into the stack.
 * @param state new state to init.
**/
void JsonState::pushState(JsonStateEnum state)
{
	JsonStateStruct tmp = {state,true};
	stateStack.push(tmp);
}

/*******************  FUNCTION  *********************/
/**
 * Pop the current state from the stack.
 * @param state State to remove (only for checking).
**/
void JsonState::popState(JsonStateEnum state)
{
	assert(stateStack.top().state == state);
	stateStack.pop();
}

/*******************  FUNCTION  *********************/
/**
 * Return the current output stream used by JsonState. It can be use for specials tricks
 * to generate outputs from stange objects, but caution, you may produce invalid
 * output if you dont check the current state.
**/
ostream& JsonState::getStream ( void )
{
	//check errors
	assert(out != NULL);
	
	//return
	return *out;
}

}
