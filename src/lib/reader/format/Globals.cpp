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
#define jsContains(value, field) ((value).find((field)) != (value).end())

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
