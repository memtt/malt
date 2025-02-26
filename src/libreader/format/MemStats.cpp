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
#include "MemStats.hpp"

/**********************************************************/
namespace MALTFormat
{

/**********************************************************/
void to_json(nlohmann::json & json, const ReallocJump & value)
{
	json = nlohmann::json{
		{"oldSize", value.oldSize},
		{"newSize", value.newSize},
		{"count", value.count},
	};
}

/**********************************************************/
void from_json(const nlohmann::json & json, ReallocJump & value)
{
	//checks
	assert(json.contains("oldSize"));
	assert(json.contains("newSize"));
	assert(json.contains("count"));

	//load
	json.at("oldSize").get_to(value.oldSize);
	json.at("newSize").get_to(value.newSize);
	json.at("count").get_to(value.count);
}

/**********************************************************/
void to_json(nlohmann::json & json, const GlobalVariable & value)
{
	json = nlohmann::json{
		{"name", value.name},
		{"size", value.size},
		{"tls", value.tls},
		{"line", value.line},
		{"file", value.file},
	};
}

/**********************************************************/
void from_json(const nlohmann::json & json, GlobalVariable & value)
{
	//checks
	assert(json.contains("name"));
	assert(json.contains("size"));
	assert(json.contains("tls"));
	assert(json.contains("line"));
	assert(json.contains("file"));

	//load
	json.at("name").get_to(value.name);
	json.at("size").get_to(value.size);
	json.at("tls").get_to(value.tls);
	json.at("line").get_to(value.line);
	json.at("file").get_to(value.file);
}

/**********************************************************/
void to_json(nlohmann::json & json, const MemStats & value)
{
	json = nlohmann::json{
		{"stack", value.sizeMap},
		{"reallocJump", value.reallocJump},
		{"globalVariables", value.globalVariables},
	};
}

/**********************************************************/
void from_json(const nlohmann::json & json, MemStats & value)
{
	//checks
	assert(json.contains("sizeMap"));
	assert(json.contains("reallocJump"));
	assert(json.contains("globalVariables"));

	//load
	json.at("sizeMap").get_to(value.sizeMap);
	json.at("reallocJump").get_to(value.reallocJump);
	json.at("globalVariables").get_to(value.globalVariables);
}

}
