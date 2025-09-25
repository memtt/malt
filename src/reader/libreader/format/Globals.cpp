/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 07/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/reader/libreader/format/Globals.cpp
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
void from_json(const JsonIn & json, Globals & value)
{
	//checks
	assert(jsContains(json, "ticksPerSecond"));
	assert(jsContains(json, "totalMemory"));
	assert(jsContains(json, "freeMemoryAtStart"));
	assert(jsContains(json, "cachedMemoryAtStart"));
	assert(jsContains(json, "maxThreadCount"));

	//load
	json.at("ticksPerSecond").get_to(value.ticksPerSecond);
	json.at("totalMemory").get_to(value.totalMemory);
	json.at("freeMemoryAtStart").get_to(value.freeMemoryAtStart);
	json.at("cachedMemoryAtStart").get_to(value.cachedMemoryAtStart);
	json.at("maxThreadCount").get_to(value.maxThreadCount);
}

}
