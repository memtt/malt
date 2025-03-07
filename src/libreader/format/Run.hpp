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
};

}

#endif //MALT_FORMAT_RUN_HPP
