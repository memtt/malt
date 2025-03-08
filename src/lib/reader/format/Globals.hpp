/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 10/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/tools/Backtrace.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

#ifndef MALT_FORMAT_GLOBALS_HPP
#define MALT_FORMAT_GLOBALS_HPP

/**********************************************************/
#include <string>
#include "Json.hpp"
#include "Types.hpp"

/**********************************************************/
namespace MALTFormat
{

/**********************************************************/
struct Globals
{
	CyclesDuration ticksPerSecond;
	size_t totalMemory;
	size_t freeMemoryAtStart;
	size_t cachedMemoryAtStart;
	size_t maxThreadCount;
};

/**********************************************************/
void from_json(const JsonIn & json, Globals & value);
void to_json(nlohmann::json & json, const Globals & value);

}

#endif //MALT_FORMAT_GLOBALS_HPP
