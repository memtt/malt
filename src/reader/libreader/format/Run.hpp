/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 07/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/reader/libreader/format/Run.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

#ifndef MALT_FORMAT_RUN_HPP
#define MALT_FORMAT_RUN_HPP

/**********************************************************/
#include <string>
#include "Types.hpp"
#include "Json.hpp"

/**********************************************************/
namespace MALTFormat
{

/**********************************************************/
struct Run
{
	std::string formatVersion;
	std::string tool;
	CyclesDuration runtime;
	std::string allocator;
	std::string exe;
	std::string command;
	std::string hostname;
	std::string date;
	std::vector<std::string> allocatorWrappers;
};

/**********************************************************/
void from_json(const JsonIn & json, Run & run);
void to_json(nlohmann::json & json, const Run & run);

}

#endif //MALT_FORMAT_RUN_HPP
