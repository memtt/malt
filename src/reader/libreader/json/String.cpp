/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 07/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/reader/libreader/json/String.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
#include <iterator>
#include <cstring>
#include "String.hpp"

/**********************************************************/
namespace MALTJson
{

/**********************************************************/
JsonString::JsonString(void)
{
}

/**********************************************************/
JsonString::JsonString(const char * input)
{
	this->value = input;
	this->len = strlen(input);
}

/**********************************************************/
std::string JsonString::toString(void) const
{
	char buffer[4096];
	char * ptr = buffer;
	if (this->len >= 4096)
		ptr = new char[this->len + 1];
	strncpy(ptr, this->value, this->len);
	ptr[this->len] = '\0';
	std::string res(ptr);
	if (ptr != buffer)
		delete [] ptr;
	return res;
}

/**********************************************************/
bool operator<(const JsonString & a, const JsonString & b)
{
	size_t commonSize = std::min(a.len, b.len);
	ssize_t result = strncmp(a.value, b.value, commonSize);
	if (result < 0)
		return true;
	else if (result == 0 && a.len < b.len)
		return true;
	else
		return false;
}

/**********************************************************/
bool operator==(const JsonString & a, const JsonString & b)
{
	if (a.len != b.len)
		return false;
	else
		return strncmp(a.value, b.value, a.len) == 0;
}

}