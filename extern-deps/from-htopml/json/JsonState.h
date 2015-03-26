/*****************************************************
             PROJECT  : htopml
             VERSION  : 0.1.0-dev
             DATE     : 03/2015
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/**********************  INFO  **********************/
/* Imported from htopml project under CeCILL-C licence */

#ifndef HTOPML_JSON_STATE_H
#define HTOPML_JSON_STATE_H

/********************  HEADERS  *********************/
//extern standard headers
#include <stack>
#include <ostream>
#include <cassert>
#include "FastBufferdStream.hpp"

/********************  NAMESPACE  *******************/
namespace htopml
{

/*********************  STRUCT  *********************/
/**
 * Define the current state for open/close checking.
 * @brief Enum to define the available json states.
**/
enum JsonStateEnum
{
	/** Currently on the root element. **/
	JSON_STATE_ROOT   = 1,
	/** Currently inside structure, expect struct end or fields. **/
	JSON_STATE_STRUCT = 2,
	/** Currently inside array, expect array end or fields. **/
	JSON_STATE_ARRAY  = 4,
	/** Currently in state field, expect value, struct or array. **/
	JSON_STATE_FIELD  = 8
};

/*********************  TYPES  **********************/
/**
 * Short structure to build the state stack inside JsonState.
 * @brief Structure to follow the json state.
**/
struct JsonStateStruct
{
	/** Current state **/
	JsonStateEnum state;
	/** Used by structure and arrays to determine the needs of separator. **/
	bool isFirst;
};

/*********************  TYPES  **********************/
/**
 * Stack of JsonStateStruct to follow the nested states while traversing the object 
 * and subobject fields. 
 * @brief Type to store the json states as a stack.
**/
typedef std::stack<JsonStateStruct> JsonStateStructStack;

/*********************  CLASS  **********************/
/**
 * JsonState is the central class to convert a structure into json text format.
 * It ensure the storage of current conversion status to open/close arrays,
 * structures and to register entries. This class is used by the convertToJson() method.
 * @brief Class to help to export objects into Json.
**/
class JsonState
{
	public:
		JsonState(std::ostream * out, bool useIndent = true, bool lua = false);
		template <class T> void printField(const char* name, const T& value);
		template <class T> void printValue(const T & value);
		template <class T> void printFieldArray(const char * name,const T * value,int size);
		template <class T> void printArray(const T * value,int size);
		void printFormattedField(const char * name,const char * format,...);
		void printFormattedValue(const char * format,...);
		void openFieldArray(const char * name);
		void closeFieldArray(const char * name);
		void openFieldStruct(const char * name);
		void closeFieldStruct(const char * name);
		void openArray(void);
		void closeArray(void);
		void openStruct(void);
		void closeStruct(void);
		void printListSeparator(void);
		JsonStateEnum getState(void) const;
		std::ostream & getStream(void);
		FastBufferedStream & getFastStream(void);
		bool isLua(void) {return lua;};
	private:
		/** Copy constructor is forbidden. **/
		JsonState(const JsonState & state);
	private:
		void openField(const char* name);
		void closeField(const char* name);
		void pushState(JsonStateEnum state);
		void popState(JsonStateEnum state);
		bool isFirst(void) const;
		void firstIsDone(void);
	private:
		void putPadding();
		/** Output stream in which to write. **/
		std::ostream * out;
		/** Indentation level. **/
		int indent;
		/** Stack of status to now in wich type of node we are. **/
		JsonStateStructStack stateStack;
		/** Enable or disable indentation. **/
		bool useIndent;
		/** Generate LUA instead of json (need to cleanup this integration). **/
		bool lua;
		FastBufferedStream bufferdStream;
};

}

/********************  HEADERS  *********************/
//include inlined template implementation
#include "JsonState_impl.h"

#endif // HTOPML_JSON_STATE_H
