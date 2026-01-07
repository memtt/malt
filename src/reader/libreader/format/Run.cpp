/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 01/2026
*    LICENSE  : CeCILL-C
*    FILE     : src/reader/libreader/format/Run.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
*    AUTHOR   : Sébastien Valat - 2026
***********************************************************/

/**********************************************************/
#include "config.h"
#include <stdexcept>
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

	//check
	if (run.formatVersion != MALT_JSON_FORMAT_VERSION) {
		char buffer[4096];
		snprintf(buffer, sizeof(buffer), "Invalid file format version, has %s where we expecte %s !", run.formatVersion.c_str(), MALT_JSON_FORMAT_VERSION);
		throw std::runtime_error(buffer);
	}

	//load
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
