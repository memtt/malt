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
#include <iostream>
#include "Threads.hpp"

/**********************************************************/
#define jsContains(value, field) ((value).find((field)) != (value).end())

/**********************************************************/
namespace MALTFormat
{

/**********************************************************/
void to_json(nlohmann::json & json, const ThreadsFuncStats & value)
{
	json = nlohmann::json{
		{"count", value.count},
		{"sum", value.sum},
		{"time", value.time},
	};
}

/**********************************************************/
void from_json(const nlohmann::json & json, ThreadsFuncStats & value)
{
	//checks
	assert(jsContains(json, "count"));
	assert(jsContains(json, "sum"));
	assert(jsContains(json, "time"));

	//load
	json.at("count").get_to(value.count);
	json.at("sum").get_to(value.sum);
	json.at("time").get_to(value.time);
}

/**********************************************************/
void to_json(nlohmann::json & json, const Thread & value)
{
	json["stats"] = nlohmann::json{
		{"malloc", value.stats.malloc},
		{"posix_memalign", value.stats.posix_memalign},
		{"aligned_alloc", value.stats.aligned_alloc},
		{"memalign", value.stats.memalign},
		{"aligned_alloc", value.stats.aligned_alloc},
		{"valloc", value.stats.valloc},
		{"pvalloc", value.stats.pvalloc},
		{"free", value.stats.free},
		{"calloc", value.stats.calloc},
		{"realloc", value.stats.realloc},
	};
	json["cntMemOps"] = value.cntMemOps;
	json["stackMem"] = nlohmann::json{ 
		{"size", value.stackMem.size},
		{"stack", value.stackMem.stack},
		{"mem", value.stackMem.mem},
		{"total", value.stackMem.total}
	};
	json["stackMem"]["timeprofiler"] = nlohmann::json{
		{"min", value.stackMem.timeprofiler.min},
		{"max", value.stackMem.timeprofiler.max},
		{"index", value.stackMem.timeprofiler.index},
		{"timestamp", value.stackMem.timeprofiler.timestamp},
		{"peakTimesteamp", value.stackMem.timeprofiler.peakTimesteamp},
		{"peakMemory", value.stackMem.timeprofiler.peakMemory},
		{"peakIndex", value.stackMem.timeprofiler.peakIndex},
		{"linearIndex", value.stackMem.timeprofiler.linearIndex},
	};
}

/**********************************************************/
void from_json(const nlohmann::json & json, Thread & value)
{
	//checks
	assert(jsContains(json, "stackMem"));
	assert(jsContains(json, "cntMemOps"));
	assert(jsContains(json, "stats"));

	//root
	json.at("cntMemOps").get_to(value.cntMemOps);

	//stats
	const nlohmann::json & stackMem = json.at("stackMem");
	assert(jsContains(stackMem, "stack"));
	assert(jsContains(stackMem, "mem"));
	assert(jsContains(stackMem, "total"));
	assert(jsContains(stackMem, "timeprofiler"));
	stackMem.at("stack").get_to(value.stackMem.stack);
	stackMem.at("mem").get_to(value.stackMem.mem);
	stackMem.at("total").get_to(value.stackMem.total);

	//timeprofiler
	const nlohmann::json & timeprofiler = stackMem.at("timeprofiler");
	assert(jsContains(timeprofiler, "min"));
	assert(jsContains(timeprofiler, "max"));
	assert(jsContains(timeprofiler, "index"));
	assert(jsContains(timeprofiler, "timestamp"));
	assert(jsContains(timeprofiler, "peakTimesteamp"));
	assert(jsContains(timeprofiler, "peakMemory"));
	assert(jsContains(timeprofiler, "peakIndex"));
	assert(jsContains(timeprofiler, "linearIndex"));
	timeprofiler.at("min").get_to(value.stackMem.timeprofiler.min);
	timeprofiler.at("max").get_to(value.stackMem.timeprofiler.max);
	timeprofiler.at("index").get_to(value.stackMem.timeprofiler.index);
	timeprofiler.at("timestamp").get_to(value.stackMem.timeprofiler.timestamp);
	timeprofiler.at("peakTimesteamp").get_to(value.stackMem.timeprofiler.peakTimesteamp);
	timeprofiler.at("peakMemory").get_to(value.stackMem.timeprofiler.peakMemory);
	timeprofiler.at("peakIndex").get_to(value.stackMem.timeprofiler.peakIndex);
	timeprofiler.at("linearIndex").get_to(value.stackMem.timeprofiler.linearIndex);

	//stats
	const nlohmann::json & stats = json.at("stats");
	assert(jsContains(stats, "malloc"));
	assert(jsContains(stats, "posix_memalign"));
	assert(jsContains(stats, "aligned_alloc"));
	assert(jsContains(stats, "memalign"));
	assert(jsContains(stats, "valloc"));
	assert(jsContains(stats, "pvalloc"));
	assert(jsContains(stats, "free"));
	assert(jsContains(stats, "calloc"));
	assert(jsContains(stats, "realloc"));
	stats.at("malloc").get_to(value.stats.malloc);
	stats.at("posix_memalign").get_to(value.stats.posix_memalign);
	stats.at("aligned_alloc").get_to(value.stats.aligned_alloc);
	stats.at("memalign").get_to(value.stats.memalign);
	stats.at("valloc").get_to(value.stats.valloc);
	stats.at("pvalloc").get_to(value.stats.pvalloc);
	stats.at("free").get_to(value.stats.free);
	stats.at("calloc").get_to(value.stats.calloc);
	stats.at("realloc").get_to(value.stats.realloc);
}

}
