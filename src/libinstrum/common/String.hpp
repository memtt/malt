/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 10/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/core/SegmentTracker.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (ECR) - 2014
*    AUTHOR   : Sébastien Valat - 2024
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
