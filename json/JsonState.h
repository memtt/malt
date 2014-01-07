/*****************************************************
             PROJECT  : htopml
             VERSION  : 0.1.0-dev
             DATE     : 11/2012
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef HTOPML_JSON_STATE_H
#define HTOPML_JSON_STATE_H

/********************  HEADERS  *********************/
#include <ostream>
#include <stack>
#include <cassert>

/********************  NAMESPACE  *******************/
namespace htopml
{

enum JsonStateEnum
{
	JSON_STATE_ROOT   = 1,
	JSON_STATE_STRUCT = 2,
	JSON_STATE_ARRAY  = 4,
	JSON_STATE_FIELD  = 8
};

/*********************  TYPES  **********************/
struct JsonStateStruct
{
	JsonStateEnum state;
	bool isFirst;
};

/*********************  TYPES  **********************/
typedef std::stack<JsonStateStruct> JsonStateStructStack;

/*********************  CLASS  **********************/
/**
 * JsonState is the central class to convert a structure into json text format.
 * It ensure the storage of current conversion status to open/close arrays,
 * structures and to register entries.
 * This class is used by the typeToJson() method.
**/
class JsonState
{
	public:
		JsonState(std::ostream * out);
		template <class T> void printField(const std::string & name,const T & value);
		template <class T> void printValue(const T & value);
		template <class T> void printFieldArray(const char * name,const T * value,int size);
		template <class T> void printArray(const T * value,int size);
		void printFormattedField(const char * name,const char * format,...);
		void printFormattedValue(const char * format,...);
		void openFieldArray(const char * name);
		void closeFieldArray(const char * name);
		void openArray(void);
		void closeArray(void);
		void openStruct(void);
		void closeStruct(void);
		JsonStateEnum getState(void) const;
	private:
		/** Copy constructor is forbidden. **/
		JsonState(const JsonState & state);
		void openField(const std::string & name);
		void closeField(const std::string & name);
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
};

}

#include "JsonState_impl.h"

#endif // HTOPML_JSON_STATE_H
