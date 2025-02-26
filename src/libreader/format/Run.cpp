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
		{"hostname", run.hostname}
	};
}

/**********************************************************/
void from_json(const nlohmann::json & json, Run & run)
{
	//checks
	assert(json.contains("formatVersion"));
	assert(json.contains("tool"));
	assert(json.contains("runtime"));
	assert(json.contains("allocator"));
	assert(json.contains("exe"));
	assert(json.contains("command"));
	assert(json.contains("hostname"));

	//load
	json.at("formatVersion").get_to(run.formatVersion);
	json.at("tool").get_to(run.tool);
	json.at("runtime").get_to(run.runtime);
	json.at("allocator").get_to(run.allocator);
	json.at("exe").get_to(run.exe);
	json.at("command").get_to(run.command);
	json.at("hostname").get_to(run.hostname);
}

}
