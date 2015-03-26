/*****************************************************
             PROJECT  : htopml
             VERSION  : 0.1.0-dev
             DATE     : 03/2015
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/**********************  INFO  **********************/
/* Imported from htopml project under CeCILL-C licence */

#ifndef HTOPML_JSON_STATE_IMPL_H
#define HTOPML_JSON_STATE_IMPL_H

/********************  HEADERS  *********************/
#include "ConvertToJson.h"

/********************  NAMESPACE  *******************/
namespace htopml
{

/*******************  FUNCTION  *********************/
/**
 * Print field based on generic convertToJson methods to convert the given object into
 * json. To be used into structures.
 * @param name Name of the field to print.
 * @param value Value to affecto the field.
**/
template <class T>
void JsonState::printField(const char * name,const T & value)
{
	//print
	openField(name);
	convertToJson(*this,value);
	closeField(name);
}

/*******************  FUNCTION  *********************/
/**
 * Print avalue based on generic convertToJson methods to convert the given object into
 * json. To be used into arrays or for the root element.
 * @param value Value to affecto the field.
**/
template <class T>
void JsonState::printValue(const T & value)
{
	//check where we are
	assert(getState() & (JSON_STATE_FIELD | JSON_STATE_ARRAY | JSON_STATE_ROOT));

	//separator
	if (getState() == JSON_STATE_ARRAY && !isFirst())
		bufferdStream << ", ";

	//print
	convertToJson(*this,value);

	firstIsDone();
}

/*******************  FUNCTION  *********************/
/**
 * Print C array as json array by using the generic template method convertToJson to
 * convert each object into json.
 * @param name Name of the field to print.
 * @param value Base address of the values array to print.
 * @param size Number of values into array.
**/
template <class T>
void JsonState::printFieldArray(const char * name,const T * value,int size)
{
	//errors
	assert(value != NULL || size == 0);
	assert(size >= 0);

	//check where we are
	assert(getState() & (JSON_STATE_STRUCT | JSON_STATE_ROOT));

	//print
	openField(name);
	openArray();
	for (int i = 0 ; i < size ; i++)
		printValue(value[i]);
	closeArray();
	closeField(name);
}

/*******************  FUNCTION  *********************/
/**
 * Print C array as json array by using the generic template method convertToJson to
 * convert each object into json.
 * @param value Base address of the values array to print.
 * @param size Number of values into array.
**/
template <class T>
void JsonState::printArray(const T * value,int size)
{
	assert(value != NULL || size == 0);
	assert(size >= 0);

	//check where we are
	assert(getState() & (JSON_STATE_FIELD | JSON_STATE_ARRAY | JSON_STATE_ROOT));

	openArray();
	for (int i = 0 ; i < size ; i++)
		printValue(value[i]);
	closeArray();
}

}

#endif // HTOPML_JSON_STATE_IMPL_H
