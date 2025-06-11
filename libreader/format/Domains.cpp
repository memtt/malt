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

	//load
	json.at("c").get_to(value.c);
	json.at("pyObj").get_to(value.pyObj);
	json.at("pyMem").get_to(value.pyMem);
	json.at("pyRaw").get_to(value.pyRaw);
}

/**********************************************************/
void to_json(nlohmann::json & json, const Domains & value)
{
	json = nlohmann::json{
		{"counters", value.counters},
		{"mem", value.mem},
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
}

}
