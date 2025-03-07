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

/**********************************************************/
namespace MALTFormat
{

/**********************************************************/
struct ProcMapEntry
{
	void * lower;
	void * upper;
	size_t offer;
	size_t aslrOffset;
	std::string file;
};

/**********************************************************/
struct InstructionInfos
{
	const char * file;
	const char * binary;
	const char * function;
	size_t line;
};

/**********************************************************/
struct Sites
{
	std::vector<ProcMapEntry> map;
	std::vector<std::string> strings;
	std::map<void*, InstructionInfos> instr;
};

}

#endif //MALT_FORMAT_SITES_HPP
