/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 10/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/tools/Backtrace.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
#include "Globals.hpp"

/**********************************************************/
namespace MALTFormat
{

/**********************************************************/
void to_json(nlohmann::json & json, const Globals & value)
{
	json = nlohmann::json{
		{"ticksPerSecond", value.ticksPerSecond},
		{"totalMemory", value.totalMemory},
		{"freeMemoryAtStart", value.freeMemoryAtStart},
		{"cachedMemoryAtStart", value.cachedMemoryAtStart},
		{"maxThreadCount", value.maxThreadCount},
	};
}

/**********************************************************/
void from_json(const nlohmann::json & json, Globals & value)
{
	//checks
	assert(json.contains("ticksPerSecond"));
	assert(json.contains("totalMemory"));
	assert(json.contains("freeMemoryAtStart"));
	assert(json.contains("cachedMemoryAtStart"));
	assert(json.contains("maxThreadCount"));

	//load
	json.at("ticksPerSecond").get_to(value.ticksPerSecond);
	json.at("totalMemory").get_to(value.totalMemory);
	json.at("freeMemoryAtStart").get_to(value.freeMemoryAtStart);
	json.at("cachedMemoryAtStart").get_to(value.cachedMemoryAtStart);
	json.at("maxThreadCount").get_to(value.maxThreadCount);
}

}
