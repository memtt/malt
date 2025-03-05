/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 10/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/tools/Backtrace.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

#ifndef MALT_FORMAT_RUN_HPP
#define MALT_FORMAT_RUN_HPP

/**********************************************************/
#include <string>
#include "Types.hpp"
#include <nlohmann/json.hpp>

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
};

/**********************************************************/
void from_json(const nlohmann::json & json, Run & run);
void to_json(nlohmann::json & json, const Run & run);

}

#endif //MALT_FORMAT_RUN_HPP
