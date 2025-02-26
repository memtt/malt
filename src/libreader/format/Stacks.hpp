/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 10/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/tools/Backtrace.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

#ifndef MALT_FORMAT_STACKS_INFOS_HPP
#define MALT_FORMAT_STACKS_INFOS_HPP

/**********************************************************/
#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include "Types.hpp"

/**********************************************************/
namespace MALTFormat
{

/**********************************************************/
struct CountMinMaxSum
{
	size_t count;
	size_t min;
	size_t max;
	size_t sum;
};

/**********************************************************/
struct StackInfos
{
	size_t countZeros;
	size_t maxAliveReq;
	size_t aliveReq;
	CountMinMaxSum alloc;
	CountMinMaxSum free;
	CountMinMaxSum lifetime;
	size_t globalPeak;
	size_t reallocCount;
	size_t reallocSumDelta;
};

/**********************************************************/
struct StackStats
{
	std::vector<LangAddress> stack;
	StackId stackId;
	StackInfos infos;
	size_t count;
};

/**********************************************************/
struct Stacks
{
	std::vector<StackStats> stats;
};

/**********************************************************/
void from_json(const nlohmann::json & json, CountMinMaxSum & value);
void to_json(nlohmann::json & json, const CountMinMaxSum & value);
void from_json(const nlohmann::json & json, StackInfos & value);
void to_json(nlohmann::json & json, const StackInfos & value);
void from_json(const nlohmann::json & json, StackStats & value);
void to_json(nlohmann::json & json, const StackStats & value);
void from_json(const nlohmann::json & json, Stacks & value);
void to_json(nlohmann::json & json, const Stacks & value);

}

#endif //MALT_FORMAT_STACKS_INFOS_HPP
