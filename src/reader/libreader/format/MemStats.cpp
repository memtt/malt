/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 05/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/reader/libreader/format/MemStats.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
#include <iostream>
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
		{"symbol", value.symbol},
		{"name", value.name},
		{"size", value.size},
		{"usedSize", value.usedSize},
		{"tls", value.tls},
		{"binaryFile", value.binaryFile},
		{"offset", value.offset},
		{"secOffset", value.secOffset},
	};

	//optionals
	if (value.line != -1)
		json["line"] = value.line;
	if (value.sourceFile != "")
		json["sourceFile"] = value.sourceFile;
}

/**********************************************************/
void from_json(const JsonIn & json, GlobalVariable & value)
{
	//checks
	assert(jsContains(json, "symbol"));
	assert(jsContains(json, "name"));
	assert(jsContains(json, "size"));
	assert(jsContains(json, "usedSize"));
	assert(jsContains(json, "tls"));
	assert(jsContains(json, "binaryFile"));
	assert(jsContains(json, "offset"));
	assert(jsContains(json, "secOffset"));

	//load
	json.at("symbol").get_to(value.symbol);
	json.at("name").get_to(value.name);
	json.at("size").get_to(value.size);
	json.at("usedSize").get_to(value.usedSize);
	json.at("tls").get_to(value.tls);
	json.at("binaryFile").get_to(value.binaryFile);
	json.at("offset").get_to(value.offset);
	json.at("secOffset").get_to(value.secOffset);

	//optionals
	if (jsContains(json, "line"))
		json.at("line").get_to(value.line);
	else
		value.line = -1;
	if (jsContains(json, "sourceFile"))
		json.at("sourceFile").get_to(value.sourceFile);
	else
		value.sourceFile = "";
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
