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
#include <cstdint>
#include <nlohmann/json.hpp>
#include "Types.hpp"

/**********************************************************/
namespace MALTFormat
{

/**********************************************************/
struct CountMinMaxSum
{
	size_t count{0};
	size_t min{SIZE_MAX};
	size_t max{0};
	size_t sum{0};
	void merge(const CountMinMaxSum & value);
};

/**********************************************************/
struct StackInfos
{
	size_t countZeros{0};
	size_t maxAliveReq{0};
	size_t aliveReq{0};
	CountMinMaxSum alloc;
	CountMinMaxSum free;
	CountMinMaxSum lifetime;
	size_t globalPeak{0};
	size_t reallocCount{0};
	size_t reallocSumDelta{0};
	void merge(const StackInfos & value);
};

/**********************************************************/
struct StackStat
{
	std::vector<LangAddress> stack;
	StackId stackId;
	StackInfos infos;
};

/**********************************************************/
typedef std::vector<StackStat> StackStats;

/**********************************************************/
struct Stacks
{
	StackStats stats;
	size_t count;
};

/**********************************************************/
void from_json(const nlohmann::json & json, CountMinMaxSum & value);
void to_json(nlohmann::json & json, const CountMinMaxSum & value);
void from_json(const nlohmann::json & json, StackInfos & value);
void to_json(nlohmann::json & json, const StackInfos & value);
void from_json(const nlohmann::json & json, StackStat & value);
void to_json(nlohmann::json & json, const StackStat & value);
void from_json(const nlohmann::json & json, Stacks & value);
void to_json(nlohmann::json & json, const Stacks & value);

}

#endif //MALT_FORMAT_STACKS_INFOS_HPP
