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
#include "Types.hpp"
#include "Sites.hpp"

/**********************************************************/
namespace MALTFormat
{

/**********************************************************/
void to_json(nlohmann::json & json, const ProcMapEntry & value)
{
	char bufferLower[256];
	char bufferUpper[256];
	snprintf(bufferLower, sizeof(bufferLower), "%p", value.lower);
	snprintf(bufferUpper, sizeof(bufferUpper), "%p", value.upper);

	json = nlohmann::json{
		{"lower", bufferLower},
		{"upper", bufferUpper},
		{"offset", value.offset},
		{"aslrOffset", value.aslrOffset},
		{"file", value.file},
	};
}

/**********************************************************/
void from_json(const nlohmann::json & json, ProcMapEntry & value)
{
	//checks
	assert(json.contains("lower"));
	assert(json.contains("upper"));
	assert(json.contains("offset"));
	assert(json.contains("aslrOffset"));
	assert(json.contains("file"));

	//load
	std::string lower;
	std::string upper;
	std::string offset;
	json.at("lower").get_to(lower);
	json.at("upper").get_to(upper);
	json.at("offset").get_to(offset);
	json.at("aslrOffset").get_to(value.aslrOffset);
	json.at("file").get_to(value.file);

	//convert
	ssize_t statusLower = sscanf(lower.c_str(), "%p", &value.lower);
	ssize_t statusUpper = sscanf(upper.c_str(), "%p", &value.upper);
	ssize_t statusOffset = sscanf(offset.c_str(), "0x%zx", &value.offset);
	assert(statusLower == 1);
	assert(statusUpper == 1);
	assert(statusOffset == 1);
}

/**********************************************************/
void to_json(nlohmann::json & json, const InstructionInfos & value)
{
	json = nlohmann::json({});
	if (value.file != -1)
		json["file"] = value.file;
	if (value.binary != -1)
		json["binary"] = value.binary;
	if (value.function != -1)
		json["function"] = value.function;
	if (value.line != -1)
		json["line"] = value.line;
}

/**********************************************************/
void from_json(const nlohmann::json & json, InstructionInfos & value)
{
	//load
	value.file = -1;
	value.binary = -1;
	value.function = -1;
	value.line = -1;
	if (json.contains("file"))
		json.at("file").get_to(value.file);
	if (json.contains("binary"))
		json.at("binary").get_to(value.binary);
	if (json.contains("function"))
		json.at("function").get_to(value.function);
	if (json.contains("line"))
		json.at("line").get_to(value.line);
}

/**********************************************************/
void to_json(nlohmann::json & json, const Sites & value)
{
	json = nlohmann::json{
		{"map", value.map},
		{"strings", value.strings},
		{"instr", value.instr},
	};
}

/**********************************************************/
void from_json(const nlohmann::json & json, Sites & value)
{
	//checks
	assert(json.contains("map"));
	assert(json.contains("strings"));
	assert(json.contains("instr"));

	//load
	json.at("map").get_to(value.map);
	json.at("strings").get_to(value.strings);
	json.at("instr").get_to(value.instr);
}

/**********************************************************/
void from_json(const nlohmann::json & json, std::map<void*, InstructionInfos> & value)
{
	for (const auto & it : json.items()) {
		void * ptr;
		std::string key = it.key();
		ssize_t status = sscanf(key.c_str(), "%p", &ptr);
		value[ptr] = it.value();
	}
}

/**********************************************************/
void to_json(nlohmann::json & json, const std::map<void*, InstructionInfos> & value)
{
	for (const auto & it : value) {
		char buffer[256];
		snprintf(buffer, sizeof(buffer), "%p", it.first);
		json[buffer] = it.second;
	}
}

}
