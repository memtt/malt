/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 10/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/tools/Backtrace.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

#ifndef MALT_FORMAT_SITES_HPP
#define MALT_FORMAT_SITES_HPP

/**********************************************************/
#include <string>
#include <vector>
#include <map>
#include <nlohmann/json.hpp>

/**********************************************************/
namespace MALTFormat
{

/**********************************************************/
struct ProcMapEntry
{
	void * lower;
	void * upper;
	size_t offset;
	size_t aslrOffset;
	std::string file;
};

/**********************************************************/
struct InstructionInfos
{
	ssize_t file;
	ssize_t binary;
	ssize_t function;
	ssize_t line;
};

/**********************************************************/
typedef std::map<LangAddress, InstructionInfos> SitesInstrMap;

/**********************************************************/
struct Sites
{
	std::vector<ProcMapEntry> map;
	std::vector<std::string> strings;
	SitesInstrMap instr;
};

/**********************************************************/
void from_json(const nlohmann::json & json, ProcMapEntry & value);
void to_json(nlohmann::json & json, const ProcMapEntry & value);
void from_json(const nlohmann::json & json, InstructionInfos & value);
void to_json(nlohmann::json & json, const InstructionInfos & value);
void from_json(const nlohmann::json & json, Sites & value);
void to_json(nlohmann::json & json, const Sites & value);
void from_json(const nlohmann::json & json, SitesInstrMap & value);
void to_json(nlohmann::json & json, const SitesInstrMap & value);

}

#endif //MALT_FORMAT_SITES_HPP
