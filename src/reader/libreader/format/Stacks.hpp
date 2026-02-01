/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 08/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/reader/libreader/format/Stacks.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

#ifndef MALT_FORMAT_STACKS_INFOS_HPP
#define MALT_FORMAT_STACKS_INFOS_HPP

/**********************************************************/
#include <string>
#include <vector>
#include <cstdint>
#include "Json.hpp"
#include "Types.hpp"

/**********************************************************/
namespace MALTFormat
{

/**********************************************************/
struct CountMinMaxSum
{
	size_t count{0};
	size_t min{0};
	size_t max{0};
	size_t sum{0};
	void merge(const CountMinMaxSum & value);
};

/**********************************************************/
struct StackInfos
{
	size_t countZeros{0};
	size_t maxAliveReq{0};
	size_t maxAliveReqGPU{0};
	size_t aliveReq{0};
	size_t aliveReqGPU{0};
	CountMinMaxSum alloc;
	CountMinMaxSum free;
	CountMinMaxSum mmap;
	CountMinMaxSum munmap;
	CountMinMaxSum lifetime;
	CountMinMaxSum gpuAlloc;
	CountMinMaxSum gpuFree;
	size_t globalPeak{0};
	size_t globalPeakGPU{0};
	size_t reallocCount{0};
	size_t reallocSumDelta{0};
	void merge(const StackInfos & value);
};

/**********************************************************/
typedef std::vector<LangAddress> Stack;

/**********************************************************/
struct StackStat
{
	Stack stack;
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
void from_json(const JsonIn & json, CountMinMaxSum & value);
void to_json(nlohmann::json & json, const CountMinMaxSum & value);
void from_json(const JsonIn & json, StackInfos & value);
void to_json(nlohmann::json & json, const StackInfos & value);
void from_json(const JsonIn & json, StackStat & value);
void to_json(nlohmann::json & json, const StackStat & value);
void from_json(const JsonIn & json, Stacks & value);
void to_json(nlohmann::json & json, const Stacks & value);

}

#endif //MALT_FORMAT_STACKS_INFOS_HPP
