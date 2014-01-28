/*****************************************************
             PROJECT  : htopml
             VERSION  : 0.1.0-dev
             DATE     : 11/2012
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
#include "ConvertToJson.h"

/**********************  USING  *********************/
using namespace std;

/********************  NAMESPACE  *******************/
namespace htopml
{

/*******************  FUNCTION  *********************/
/**
 * Specific implementation for std::string. Required to escape backslashes.
 * @param json The json state used to procude the json output.
 * @param value The string to convert.
**/
void convertToJson(JsonState & json, const std::string& value)
{
	return convertToJson(json,value.c_str());
}

/*******************  FUNCTION  *********************/
/**
 * Specific implementation for C strings. Required to escape backslashes.
 * @param json The json state used to procude the json output.
 * @param value The string to convert.
**/
void convertToJson(JsonState & json, const char* value)
{
	std::ostream & out = json.getStream();
	out << '\"';
	while (value != NULL && *value != '\0')
	{
		if (*value == '\"')
			out << "\\\"";
		else
			out << *value;
		value++;
	}
	out << '"';
}

/*******************  FUNCTION  *********************/
void convertToJson(JsonState & json, unsigned long value)
{
	json.getStream() << value;
}

/*******************  FUNCTION  *********************/
void convertToJson(JsonState & json, long value)
{
	json.getStream() << value;
}

/*******************  FUNCTION  *********************/
void convertToJson(JsonState & json, double value)
{
	json.getStream() << value;
}

/*******************  FUNCTION  *********************/
/**
 * Specific implementation to convert to "true" or "false" string compatible with json format.
 * 
**/
void convertToJson(JsonState & json, bool value)
{
	if (value)
		json.getStream() << "true";
	else
		json.getStream() << "false";
}

/*******************  FUNCTION  *********************/
void convertToJson(JsonState& json, void * ptr)
{
	json.getStream() << '"' << ptr << '"';
}

}
