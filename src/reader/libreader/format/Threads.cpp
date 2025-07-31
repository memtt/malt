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
void from_json(const JsonIn & json, ThreadsFuncStats & value)
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
void from_json(const JsonIn & json, ThreadStackMem & value)
{
	//stats
	const JsonIn & stackMem = json;
	assert(jsContains(stackMem, "size"));
	assert(jsContains(stackMem, "stack"));
	assert(jsContains(stackMem, "mem"));
	assert(jsContains(stackMem, "total"));
	assert(jsContains(stackMem, "timeprofiler"));
	stackMem.at("size").get_to(value.size);
	stackMem.at("stack").get_to(value.stack);
	stackMem.at("mem").get_to(value.mem);
	stackMem.at("total").get_to(value.total);

	//timeprofiler
	const JsonIn & timeprofiler = stackMem.at("timeprofiler");
	assert(jsContains(timeprofiler, "min"));
	assert(jsContains(timeprofiler, "max"));
	assert(jsContains(timeprofiler, "index"));
	assert(jsContains(timeprofiler, "timestamp"));
	assert(jsContains(timeprofiler, "peakTimesteamp"));
	assert(jsContains(timeprofiler, "peakMemory"));
	assert(jsContains(timeprofiler, "peakIndex"));
	assert(jsContains(timeprofiler, "linearIndex"));
	timeprofiler.at("min").get_to(value.timeprofiler.min);
	timeprofiler.at("max").get_to(value.timeprofiler.max);
	timeprofiler.at("index").get_to(value.timeprofiler.index);
	timeprofiler.at("timestamp").get_to(value.timeprofiler.timestamp);
	timeprofiler.at("peakTimesteamp").get_to(value.timeprofiler.peakTimesteamp);
	timeprofiler.at("peakMemory").get_to(value.timeprofiler.peakMemory);
	timeprofiler.at("peakIndex").get_to(value.timeprofiler.peakIndex);
	timeprofiler.at("linearIndex").get_to(value.timeprofiler.linearIndex);
	if (jsContains(timeprofiler, "location"))
		timeprofiler.at("location").get_to(value.timeprofiler.location);
}

/**********************************************************/
void to_json(nlohmann::json & json, const ThreadStackMem & value)
{
	json = nlohmann::json{ 
		{"size", value.size},
		{"stack", value.stack},
		{"mem", value.mem},
		{"total", value.total}
	};
	json["timeprofiler"] = nlohmann::json{
		{"min", value.timeprofiler.min},
		{"max", value.timeprofiler.max},
		{"index", value.timeprofiler.index},
		{"timestamp", value.timeprofiler.timestamp},
		{"peakTimesteamp", value.timeprofiler.peakTimesteamp},
		{"peakMemory", value.timeprofiler.peakMemory},
		{"peakIndex", value.timeprofiler.peakIndex},
		{"linearIndex", value.timeprofiler.linearIndex},
		{"location", value.timeprofiler.location},
	};
}

/**********************************************************/
void from_json(const JsonIn & json, ThreadsStats & value)
{
	//stats
	const JsonIn & stats = json;
	assert(jsContains(stats, "malloc"));
	assert(jsContains(stats, "posix_memalign"));
	assert(jsContains(stats, "aligned_alloc"));
	assert(jsContains(stats, "memalign"));
	assert(jsContains(stats, "valloc"));
	assert(jsContains(stats, "pvalloc"));
	assert(jsContains(stats, "free"));
	assert(jsContains(stats, "calloc"));
	assert(jsContains(stats, "realloc"));
	stats.at("malloc").get_to(value.malloc);
	stats.at("posix_memalign").get_to(value.posix_memalign);
	stats.at("aligned_alloc").get_to(value.aligned_alloc);
	stats.at("memalign").get_to(value.memalign);
	stats.at("valloc").get_to(value.valloc);
	stats.at("pvalloc").get_to(value.pvalloc);
	stats.at("free").get_to(value.free);
	stats.at("calloc").get_to(value.calloc);
	stats.at("realloc").get_to(value.realloc);
}

/**********************************************************/
void to_json(nlohmann::json & json, const ThreadsStats & value)
{
	json = nlohmann::json{
		{"malloc", value.malloc},
		{"posix_memalign", value.posix_memalign},
		{"aligned_alloc", value.aligned_alloc},
		{"memalign", value.memalign},
		{"aligned_alloc", value.aligned_alloc},
		{"valloc", value.valloc},
		{"pvalloc", value.pvalloc},
		{"free", value.free},
		{"calloc", value.calloc},
		{"realloc", value.realloc},
	};
}

/**********************************************************/
void to_json(nlohmann::json & json, const Thread & value)
{
	json["stats"] = value.stats;
	json["cntMemOps"] = value.cntMemOps;
	json["stackMem"] = value.stackMem;
}

/**********************************************************/
void from_json(const JsonIn & json, Thread & value)
{
	//checks
	assert(jsContains(json, "stackMem"));
	assert(jsContains(json, "cntMemOps"));
	assert(jsContains(json, "stats"));

	//root
	json.at("cntMemOps").get_to(value.cntMemOps);

	//stackMem
	json.at("stackMem").get_to(value.stackMem);

	//stats
	json.at("stats").get_to(value.stats);
}

}
