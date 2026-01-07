/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 05/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/reader/libreader/format/MemStats.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

#ifndef MALT_FORMAT_MEM_STATS_HPP
#define MALT_FORMAT_MEM_STATS_HPP

/**********************************************************/
#include <string>
#include <vector>
#include <map>
#include "Json.hpp"
#include "Types.hpp"

/**********************************************************/
namespace MALTFormat
{

/**********************************************************/
struct ReallocJump
{
	size_t oldSize;
	size_t newSize;
	size_t count;
};

/**********************************************************/
struct GlobalVariable
{
	std::string symbol;
	std::string name;
	size_t size;
	size_t usedSize;
	size_t offset;
	size_t secOffset;
	bool tls;
	ssize_t line;
	std::string sourceFile;
	std::string binaryFile;
};

/**********************************************************/
typedef std::map<size_t, size_t> MemStatsSizeMap;

/**********************************************************/
struct MemStats
{
	MemStatsSizeMap sizeMap;
	std::vector<ReallocJump> reallocJump;
	std::map<std::string, std::vector<GlobalVariable> > globalVariables;
};

/**********************************************************/
void from_json(const JsonIn & json, ReallocJump & value);
void to_json(nlohmann::json & json, const ReallocJump & value);
void from_json(const JsonIn & json, GlobalVariable & value);
void to_json(nlohmann::json & json, const GlobalVariable & value);
void from_json(const JsonIn & json, MemStats & value);
void to_json(nlohmann::json & json, const MemStats & value);
void from_json(const JsonIn & json, MemStatsSizeMap & value);
void to_json(nlohmann::json & json, const MemStatsSizeMap & value);

}

#endif //MALT_FORMAT_MEM_STATS_HPP
