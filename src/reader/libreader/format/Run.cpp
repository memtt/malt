/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 07/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/reader/libreader/format/Run.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
#include "Run.hpp"

/**********************************************************/
namespace MALTFormat
{

/**********************************************************/
void to_json(nlohmann::json & json, const Run & run)
{
	json = nlohmann::json{
		{"formatVersion", run.formatVersion},
		{"tool", run.tool},
		{"runtime", run.runtime},
		{"allocator", run.allocator},
		{"exe", run.exe},
		{"command", run.command},
		{"hostname", run.hostname},
		{"date", run.date},
		{"allocatorWrappers", run.allocatorWrappers}
	};
}

/**********************************************************/
void from_json(const JsonIn & json, Run & run)
{
	//checks
	assert(jsContains(json, "formatVersion"));
	assert(jsContains(json, "tool"));
	assert(jsContains(json, "runtime"));
	assert(jsContains(json, "allocator"));
	assert(jsContains(json, "exe"));
	assert(jsContains(json, "command"));
	assert(jsContains(json, "hostname"));
	assert(jsContains(json, "date"));

	//load
	json.at("formatVersion").get_to(run.formatVersion);
	json.at("tool").get_to(run.tool);
	json.at("runtime").get_to(run.runtime);
	json.at("allocator").get_to(run.allocator);
	json.at("exe").get_to(run.exe);
	json.at("command").get_to(run.command);
	json.at("hostname").get_to(run.hostname);
	json.at("date").get_to(run.date);
	if (jsContains(json, "allocatorWrappers"))
		json.at("allocatorWrappers").get_to(run.allocatorWrappers);
}

}
