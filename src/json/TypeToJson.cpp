/*****************************************************
             PROJECT  : htopml
             VERSION  : 0.1.0-dev
             DATE     : 11/2012
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
#include "TypeToJson.h"

/**********************  USING  *********************/
using namespace std;

/********************  NAMESPACE  *******************/
namespace htopml
{

/*******************  FUNCTION  *********************/
void typeToJson(JsonState & json,std::ostream& stream, const std::string& value)
{
	return typeToJson(json,stream,value.c_str());
}

/*******************  FUNCTION  *********************/
void typeToJson(JsonState & json,std::ostream& stream, const char* value)
{
	stream << '\"';
	while (value != NULL && *value != '\0')
	{
		if (*value == '\"')
			stream << "\\\"";
		else
			stream << *value;
		value++;
	}
	stream << '"';
}

/*******************  FUNCTION  *********************/
void typeToJson(JsonState & json,std::ostream& stream, unsigned long value)
{
	stream << value;
}

/*******************  FUNCTION  *********************/
void typeToJson(JsonState & json,std::ostream& stream, long value)
{
	stream << value;
}

/*******************  FUNCTION  *********************/
void typeToJson(JsonState & json,std::ostream& stream, double value)
{
	stream << value;
}

void typeToJson(JsonState & json,std::ostream& stream, bool value)
{
	if (value)
		stream << "true";
	else
		stream << "false";
}

}
