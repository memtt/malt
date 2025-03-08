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
void from_json(const JsonIn & json, ReallocJump & value)
{
	//checks
	assert(jsContains(json, "oldSize"));
	assert(jsContains(json, "newSize"));
	assert(jsContains(json, "count"));

	//load
	json.at("oldSize").get_to(value.oldSize);
	json.at("newSize").get_to(value.newSize);
	json.at("count").get_to(value.count);
}

/**********************************************************/
void to_json(nlohmann::json & json, const GlobalVariable & value)
{
	//defaults
	json = nlohmann::json{
		{"name", value.name},
		{"size", value.size},
		{"tls", value.tls},
	};

	//optionals
	if (value.line != -1)
		json["line"] = value.line;
	if (value.file != "")
		json["file"] = value.file;
}

/**********************************************************/
void from_json(const JsonIn & json, GlobalVariable & value)
{
	//checks
	assert(jsContains(json, "name"));
	assert(jsContains(json, "size"));
	assert(jsContains(json, "tls"));

	//load
	json.at("name").get_to(value.name);
	json.at("size").get_to(value.size);
	json.at("tls").get_to(value.tls);

	//optionals
	if (jsContains(json, "line"))
		json.at("line").get_to(value.line);
	else
		value.line = -1;
	if (jsContains(json, "file"))
		json.at("file").get_to(value.file);
	else
		value.file = "";
}


/**********************************************************/
void to_json(nlohmann::json & json, const MemStats & value)
{
	json = nlohmann::json{
		{"reallocJump", value.reallocJump},
		{"globalVariables", value.globalVariables},
	};
	to_json(json["sizeMap"], value.sizeMap);
}

/**********************************************************/
void from_json(const JsonIn & json, MemStats & value)
{
	//checks
	assert(jsContains(json, "sizeMap"));
	assert(jsContains(json, "reallocJump"));
	assert(jsContains(json, "globalVariables"));

	//load
	from_json(json.at("sizeMap"), value.sizeMap);
	//json.at("sizeMap").get_to(value.sizeMap);
	json.at("reallocJump").get_to(value.reallocJump);
	json.at("globalVariables").get_to(value.globalVariables);
}

/**********************************************************/
void from_json(const JsonIn & json, MemStatsSizeMap & value)
{
	for (const auto & it : json.items()) {
		//extract
		const size_t key = atol(it.key().c_str());
		size_t val;
		it.value().get_to(val);
		value[key] = val;
	}
}

/**********************************************************/
void to_json(nlohmann::json & json, const MemStatsSizeMap & value)
{
	for (const auto & it : value) {
		char buffer[256];
		snprintf(buffer, sizeof(buffer), "%zu", it.first);
		json[buffer] = it.second;
	}
}

}
