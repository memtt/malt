/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 10/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/tools/Backtrace.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

#ifndef MALT_FORMAT_LEAKS_HPP
#define MALT_FORMAT_LEAKS_HPP

/**********************************************************/
#include <string>
#include <vector>
#include "Types.hpp"

/**********************************************************/
namespace MALTFormat
{

/**********************************************************/
struct Leak
{
	std::vector<LangAddress> stack;
	size_t count;
	size_t memory;
};

/**********************************************************/
typedef std::vector<Leak> Leaks;

}

#endif //MALT_FORMAT_LEAKS_HPP
