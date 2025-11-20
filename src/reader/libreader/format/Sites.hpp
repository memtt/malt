/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 10/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/reader/libreader/format/Sites.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

#ifndef MALT_FORMAT_SITES_HPP
#define MALT_FORMAT_SITES_HPP

/**********************************************************/
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include "Json.hpp"

/**********************************************************/
template<>
struct std::hash<MALTFormat::LangAddress> {
    std::size_t operator()(MALTFormat::LangAddress const& s) const noexcept {
        return (size_t)s.address;
    }
};

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
typedef std::unordered_map<LangAddress, InstructionInfos> SitesInstrMap;

/**********************************************************/
struct Sites
{
	std::vector<ProcMapEntry> map;
	std::vector<std::string> strings;
	SitesInstrMap instr;
};

/**********************************************************/
void from_json(const JsonIn & json, ProcMapEntry & value);
void to_json(nlohmann::json & json, const ProcMapEntry & value);
void from_json(const JsonIn & json, InstructionInfos & value);
void to_json(nlohmann::json & json, const InstructionInfos & value);
void from_json(const JsonIn & json, Sites & value);
void to_json(nlohmann::json & json, const Sites & value);
void from_json(const JsonIn & json, SitesInstrMap & value);
void to_json(nlohmann::json & json, const SitesInstrMap & value);

}

#endif //MALT_FORMAT_SITES_HPP
