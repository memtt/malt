/*****************************************************
             PROJECT  : htopml
             VERSION  : 0.1.0-dev
             DATE     : 11/2012
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/**********************  INFO  **********************/
/* Imported from htopml project under CeCILL-C licence */

#ifndef HTOPML_TYPE_TO_JSON_IMPL_H
#define HTOPML_TYPE_TO_JSON_IMPL_H

/********************  HEADERS  *********************/
#include "ConvertToJson.h"
#include "JsonState.h"

/********************  NAMESPACE  *******************/
namespace htopml
{

/*******************  FUNCTION  *********************/
template <class T> void convertToJson(JsonState & json, const std::vector<T> & iterable)
{
	json.openArray();

	for (typename std::vector<T>::const_iterator it = iterable.begin() ; it != iterable.end() ; ++it)
		json.printValue(*it);

	json.closeArray();
}

/*******************  FUNCTION  *********************/
template <class T> void convertToJson(JsonState & json, const std::list<T> & iterable)
{
	json.openArray();

	for (typename std::list<T>::const_iterator it = iterable.begin() ; it != iterable.end() ; ++it)
		json.printValue(*it);

	json.closeArray();
}

/*******************  FUNCTION  *********************/
template <class T,class U> void convertToJson(JsonState & json, const std::map<T,U> & iterable)
{
	json.openStruct();

	for (typename std::map<T,U>::const_iterator it = iterable.begin() ; it != iterable.end() ; ++it)
		json.printField(it->first,it->second);

	json.closeStruct();
}

/*******************  FUNCTION  *********************/
template <class T> void convertToJson(JsonState & json, const T & value)
{
	json.getStream() << value;
}

/*******************  FUNCTION  *********************/
template <class T> std::ostream& convertToJson(std::ostream& out,const T & value, bool indent)
{
	JsonState state(&out,indent);
	state.printValue(value);
	return out;
}

/*******************  FUNCTION  *********************/
template <class T> std::ostream& convertToJson(std::ostream& out,const T * value,bool indent)
{
	JsonState state(&out,indent);
	if (value == NULL)
		state.printFormattedValue("null");
	else
		state.printValue(*value);
	return out;
}

/*******************  FUNCTION  *********************/
template <class T> std::ostream& convertToLua(std::ostream& out,const T & value, bool indent)
{
	out << "mattProfile=";
	JsonState state(&out,indent,true);
	state.printValue(value);
	return out;
}

/*******************  FUNCTION  *********************/
inline void convertToJson ( JsonState& json, IJsonConvertible& object )
{
	object.toJson(json);
}

}

#endif //TYPE_TO_JSON_H
