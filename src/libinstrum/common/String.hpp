/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.3.1
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
/**
 * @brief Define a string which is backed on top of the internal allocator.
 */
typedef std::basic_string<char, std::char_traits<char>, STLInternalAllocator<char> > String;

/**********************************************************/
/**
 * @brief Handle putting the MALT string into json rendering pipeline.
 * @param json The json object to feed.
 * @param value The string value to push.
 */
inline void convertToJson(htopml::JsonState & json, const String & value)
{
	json.printValue(std::string(value.c_str()));
}

}

#endif //MALT_STRING_HPP
