/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 10/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/tools/Backtrace.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
#include "Types.hpp"
#include "Stacks.hpp"

/**********************************************************/
namespace MALTFormat
{

/**********************************************************/
void to_json(nlohmann::json & json, const CountMinMaxSum & value)
{
	json = nlohmann::json{
		{"count", value.count},
		{"min", value.min},
		{"max", value.max},
		{"sum", value.sum},
	};
}

/**********************************************************/
void from_json(const nlohmann::json & json, CountMinMaxSum & value)
{
	//checks
	assert(json.contains("count"));
	assert(json.contains("min"));
	assert(json.contains("max"));
	assert(json.contains("sum"));

	//load
	json.at("count").get_to(value.count);
	json.at("min").get_to(value.min);
	json.at("max").get_to(value.max);
	json.at("sum").get_to(value.sum);
}

/**********************************************************/
void to_json(nlohmann::json & json, const StackInfos & value)
{
	json = nlohmann::json{
		{"countZeros", value.countZeros},
		{"maxAliveReq", value.maxAliveReq},
		{"aliveReq", value.aliveReq},
		{"alloc", value.alloc},
		{"free", value.free},
		{"lifetime", value.lifetime},
		{"globalPeak", value.globalPeak},
		{"reallocCount", value.reallocCount},
		{"reallocSumDelta", value.reallocSumDelta},
	};
}

/**********************************************************/
void from_json(const nlohmann::json & json, StackInfos & value)
{
	//checks
	assert(json.contains("countZeros"));
	assert(json.contains("maxAliveReq"));
	assert(json.contains("aliveReq"));
	assert(json.contains("alloc"));
	assert(json.contains("free"));
	assert(json.contains("lifetime"));
	assert(json.contains("globalPeak"));
	assert(json.contains("reallocCount"));
	assert(json.contains("reallocSumDelta"));

	//load
	json.at("countZeros").get_to(value.countZeros);
	json.at("maxAliveReq").get_to(value.maxAliveReq);
	json.at("aliveReq").get_to(value.aliveReq);
	json.at("alloc").get_to(value.alloc);
	json.at("free").get_to(value.free);
	json.at("lifetime").get_to(value.lifetime);
	json.at("globalPeak").get_to(value.globalPeak);
	json.at("reallocCount").get_to(value.reallocCount);
	json.at("reallocSumDelta").get_to(value.reallocSumDelta);
}

/**********************************************************/
void to_json(nlohmann::json & json, const StackStat & value)
{
	char bufferStackId[256];
	if (value.stackId == nullptr)
		snprintf(bufferStackId, sizeof(bufferStackId), "0x0");
	else
		snprintf(bufferStackId, sizeof(bufferStackId), "%p", value.stackId);
	json = nlohmann::json{
		{"stack", value.stack},
		{"stackId", bufferStackId},
		{"infos", value.infos},
	};
}

/**********************************************************/
void from_json(const nlohmann::json & json, StackStat & value)
{
	//checks
	assert(json.contains("stack"));
	assert(json.contains("stackId"));
	assert(json.contains("infos"));

	//load
	std::string stackId;
	json.at("stack").get_to(value.stack);
	json.at("stackId").get_to(stackId);
	json.at("infos").get_to(value.infos);

	//convert
	ssize_t status = sscanf(stackId.c_str(), "%p", &value.stackId);
	assert(status == 1);
}

/**********************************************************/
void to_json(nlohmann::json & json, const Stacks & value)
{
	json = nlohmann::json{
		{"stats", value.stats},
		{"count", value.count},
	};
}

/**********************************************************/
void from_json(const nlohmann::json & json, Stacks & value)
{
	//check
	assert(json.contains("stats"));
	assert(json.contains("count"));
	//json.at("stats").get_to(value.stats);
	json.at("count").get_to(value.count);

	//get it
	const nlohmann::json & stats = json["stats"];
	assert(stats.is_array());
	value.stats.reserve(stats.size());
	for (size_t i = 0 ; i < stats.size() ; i++) {
		value.stats.emplace_back();
		//#pragma omp task
		stats[i].get_to(value.stats[i]);
	}
}

/**********************************************************/
void CountMinMaxSum::merge(const CountMinMaxSum & value)
{
	//first fill
	if (this->count == 0 && this->min == 0)
		this->min = value.min;

	//make normal
	this->count += value.count;
	this->sum += value.sum;
	this->min = std::min(this->min, value.min);
	this->max = std::max(this->max, value.max);
}

/**********************************************************/
void StackInfos::merge(const StackInfos & value)
{
	this->countZeros += value.countZeros;
	this->maxAliveReq += value.maxAliveReq;
	this->aliveReq += value.aliveReq;
	this->globalPeak += value.globalPeak;
	this->reallocCount += value.reallocCount;
	this->reallocSumDelta += value.reallocSumDelta;
	this->alloc.merge(value.alloc);
	this->free.merge(value.free);
	this->lifetime.merge(value.lifetime);
}

}
