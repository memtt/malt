/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.3.1
*    DATE     : 03/2025
*    LICENSE  : CeCILL-C
*    FILE     : ./src/reader/libreader/json/String.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
#ifndef MALT_JSON_STRING_HPP
#define MALT_JSON_STRING_HPP

/**********************************************************/
#include <cstdlib>
#include <string>

/**********************************************************/
namespace MALTJson
{

/**********************************************************/
struct JsonString
{
	JsonString(void);
	JsonString(const char * input);
	const char * value{nullptr};
	size_t len{0};
	std::string toString(void) const;
};

/**********************************************************/
bool operator<(const JsonString & a, const JsonString & b);
bool operator==(const JsonString & a, const JsonString & b);

}

#endif //MALT_JSON_STRING_HPP
