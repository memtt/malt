/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 10/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/tools/Backtrace.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

#ifndef MALT_FORMAT_MEM_STATS_HPP
#define MALT_FORMAT_MEM_STATS_HPP

/**********************************************************/
#include <string>
#include <vector>
#include <map>
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
	std::string name;
	size_t size;
	bool tls;
	size_t line;
	size_t file;
};

/**********************************************************/
struct MemStats
{
	std::map<size_t, size_t> sizeMap;
	ReallocJump reallocJump;
	std::map<std::string, std::vector<GlobalVariable> > globalVariables;
};

}

#endif //MALT_FORMAT_MEM_STATS_HPP
