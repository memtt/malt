/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 10/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/reader/libreader/format/Domains.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
#include "Domains.hpp"
#include <cassert>

/**********************************************************/
namespace MALTFormat
{

/**********************************************************/
void to_json(nlohmann::json & json, const DomainCounters & value)
{
	json = nlohmann::json{
		{"c", value.c},
		{"pyObj", value.pyObj},
		{"pyMem", value.pyMem},
		{"pyRaw", value.pyRaw},
		{"mmap", value.mmap},
		{"gpu", value.gpu},
	};
}

/**********************************************************/
void from_json(const JsonIn & json, DomainCounters & value)
{
	//checks
	assert(jsContains(json, "c"));
	assert(jsContains(json, "pyObj"));
	assert(jsContains(json, "pyMem"));
	assert(jsContains(json, "pyRaw"));
	assert(jsContains(json, "mmap"));
	assert(jsContains(json, "gpu"));

	//load
	json.at("c").get_to(value.c);
	json.at("pyObj").get_to(value.pyObj);
	json.at("pyMem").get_to(value.pyMem);
	json.at("pyRaw").get_to(value.pyRaw);
	json.at("mmap").get_to(value.mmap);
	json.at("gpu").get_to(value.gpu);
}

/**********************************************************/
void to_json(nlohmann::json & json, const Domains & value)
{
	json = nlohmann::json{
		{"counters", value.counters},
		{"mem", value.mem},
		{"atGlobalPeak", value.atGlobalPeak},
		{"localPeak", value.localPeak},
	};
}

/**********************************************************/
void from_json(const JsonIn & json, Domains & value)
{
	//checks
	assert(jsContains(json, "counters"));
	assert(jsContains(json, "mem"));

	//load
	json.at("counters").get_to(value.counters);
	json.at("mem").get_to(value.mem);
	json.at("atGlobalPeak").get_to(value.atGlobalPeak);
	json.at("localPeak").get_to(value.localPeak);
}

}
