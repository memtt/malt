/*****************************************************
             PROJECT  : htopml
             VERSION  : 0.1.0-dev
             DATE     : 11/2012
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef HTOPML_TYPE_TO_JSON_H
#define HTOPML_TYPE_TO_JSON_H

/********************  HEADERS  *********************/
//std
#include <map>
#include <list>
#include <string>
#include <vector>
#include <ostream>
//internal
#include "JsonState.h"

/********************  NAMESPACE  *******************/
namespace htopml
{

/*********************  CLASS  **********************/
class IJsonConvertible
{
	public:
		virtual void toJson(JsonState & json) = 0;
};

/********************  MACRO  ***********************/
/**
 * Short macro to quickly implement some type alias fo convertToJson base on another type.
 * It is usefull to avoid compiler warnings about implicit conversions and to not fully
 * reimplement each types manually when it can be cast safetly.
 * @param aliasType Type for which to define a new convertToJson() implementation.
 * @param targetType Convert to targetType and call the related implementation of convertToJson() function.
**/
#define HTOPML_TYPE_TO_JSON_BASIC_TYPE(aliasType,targetType) \
	static inline void convertToJson(JsonState & json, aliasType value) {return convertToJson(json,(targetType)value);}

/*******************  FUNCTION  *********************/
/**
 * Generic implementation for the final user to convert an object to json document.
 * To be supported, each object of you tree need to provide standard output stream operator
 * or implement a specific version of convertToJson(JsonState &,T & value).
 * @param out Define the output stream into which to print the json output.
 * @param value Reference to the object to convert.
**/
template <class T> std::ostream& convertToJson(std::ostream& out,const T & value);
/**
 * Generic implementation for the final user to convert an object to json document.
 * To be supported, each object of you tree need to provide standard output stream operator
 * or implement a specific version of convertToJson(JsonState &,T & value).
 * @param out Define the output stream into which to print the json output.
 * @param value Pointer to the object to convert.
**/
template <class T> std::ostream& convertToJson(std::ostream& out,const T * value);

/*******************  FUNCTION  *********************/
//specific implementations for some known types
void convertToJson(JsonState & json, bool value);
void convertToJson(JsonState & json, void * ptr);
void convertToJson(JsonState & json, IJsonConvertible & object);

/*******************  FUNCTION  *********************/
//generic version
template <class T> void convertToJson(JsonState & json, const T & iterable);

/*******************  FUNCTION  *********************/
//specific implementation for some STL containers
template <class T> void convertToJson(JsonState & json, const std::vector<T> & iterable);
template <class T> void convertToJson(JsonState & json, const std::list<T> & iterable);
template <class T,class U> void convertToJson(JsonState & json, const std::map<T,U> & iterable);

}

/********************  HEADERS  *********************/
//import the inline templates implementation
#include "ConvertToJson_impl.h"

#endif //HTOPML_TYPE_TO_JSON_H
