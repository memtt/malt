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
		{"stack", value.count},
		{"count", value.min},
		{"memory", value.max},
		{"memory", value.sum},
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
		{"stack", value.countZeros},
		{"memory", value.maxAliveReq},
		{"memory", value.aliveReq},
		{"memory", value.alloc},
		{"memory", value.free},
		{"memory", value.lifetime},
		{"memory", value.globalPeak},
		{"memory", value.reallocCount},
		{"memory", value.reallocSumDelta},
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
void to_json(nlohmann::json & json, const StackStats & value)
{
	char bufferStackId[256];
	snprintf(bufferStackId, sizeof(bufferStackId), "0x%p", value.stackId);
	json = nlohmann::json{
		{"stack", value.stack},
		{"stackId", bufferStackId},
		{"infos", value.infos},
		{"count", value.count},
	};
}

/**********************************************************/
void from_json(const nlohmann::json & json, StackStats & value)
{
	//checks
	assert(json.contains("stack"));
	assert(json.contains("stackId"));
	assert(json.contains("infos"));
	assert(json.contains("count"));

	//load
	std::string stackId;
	json.at("stack").get_to(value.stack);
	json.at("stackId").get_to(stackId);
	json.at("infos").get_to(value.infos);
	json.at("count").get_to(value.count);

	//convert
	ssize_t status = sscanf(stackId.c_str(), "0x%p", &value.stackId);
	assert(status == 1);
}

/**********************************************************/
void to_json(nlohmann::json & json, const Stacks & value)
{
	json = nlohmann::json{
		{"stats", value.stats},
	};
}

/**********************************************************/
void from_json(const nlohmann::json & json, Stacks & value)
{
	//checks
	assert(json.contains("stats"));

	//load
	json.at("stats").get_to(value.stats);
}

}
