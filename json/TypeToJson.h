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
#include <ostream>
#include <string>
#include <vector>
#include <list>
#include <map>
#include "JsonState.h"

/********************  NAMESPACE  *******************/
namespace htopml
{

/********************  MACRO  ***********************/
#define IHSTK_TYPE_TO_JSON_BASIC_TYPE(alias_type,target_type) \
	static inline void typeToJson(JsonState & json,std::ostream& stream, alias_type value) {return typeToJson(json,stream,(target_type)value);}

/*******************  FUNCTION  *********************/
template <class T> std::ostream& typeToJson(std::ostream& out,const T & value);
template <class T> std::ostream& typeToJson(std::ostream& out,const T * value);

/*******************  FUNCTION  *********************/
void typeToJson(JsonState & json,std::ostream& stream, const std::string& value);
void typeToJson(JsonState & json,std::ostream& stream, const char * value);
void typeToJson(JsonState & json,std::ostream& stream, long value);
void typeToJson(JsonState & json,std::ostream& stream, unsigned long value);
void typeToJson(JsonState & json,std::ostream& stream, double value);
void typeToJson(JsonState & json,std::ostream& stream, bool value);

/*******************  FUNCTION  *********************/
template <class T> void typeToJson(JsonState & json,std::ostream& stream, const std::vector<T> & iterable);
template <class T> void typeToJson(JsonState & json,std::ostream& stream, const std::list<T> & iterable);
template <class T,class U> void typeToJson(JsonState & json,std::ostream& stream, const std::map<T,U> & iterable);

/*******************  FUNCTION  *********************/
IHSTK_TYPE_TO_JSON_BASIC_TYPE(int,long)
IHSTK_TYPE_TO_JSON_BASIC_TYPE(unsigned int,unsigned long)
IHSTK_TYPE_TO_JSON_BASIC_TYPE(float,double)

}

#include "TypeToJson_impl.h"

#endif //HTOPML_TYPE_TO_JSON_H
