/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.6
*    DATE     : 04/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/common/String.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

#ifndef MALT_STRING_HPP
#define MALT_STRING_HPP

/**********************************************************/
#include <string>
#include "STLInternalAllocator.hpp"
#include <json/JsonState.h>

/**********************************************************/
namespace MALT
{

/**********************************************************/
typedef std::basic_string<char, std::char_traits<char>, STLInternalAllocator<char> > String;

/**********************************************************/
inline void convertToJson(htopml::JsonState & json, const String & value)
{
	json.printValue(std::string(value.c_str()));
}

}

#endif //MALT_STRING_HPP
