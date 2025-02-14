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

/**********************************************************/
namespace MALT
{

/**********************************************************/
typedef std::basic_string<char, std::char_traits<char>, STLInternalAllocator<char> > String;

}

#endif //MALT_STRING_HPP
