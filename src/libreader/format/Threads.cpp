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
void from_json(const nlohmann::json & json, ThreadsFuncStats & value)
{
	//checks
	assert(json.contains("count"));
	assert(json.contains("sum"));
	assert(json.contains("time"));

	//load
	json.at("count").get_to(value.count);
	json.at("sum").get_to(value.sum);
	json.at("time").get_to(value.time);
}

/**********************************************************/
void to_json(nlohmann::json & json, const Thread & value)
{
	json = nlohmann::json{
			{"stats", {
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
			},
			{"cntMemOps", value.cntMemOps},
			{"stackMem", 
				{"size", value.stackMem.size},
				{"stack", value.stackMem.stack},
				{"mem", value.stackMem.mem},
				{"total", value.stackMem.total},
				{"timeprofiler",
					{"min", value.stackMem.timeprofiler.min},
					{"max", value.stackMem.timeprofiler.max},
					{"index", value.stackMem.timeprofiler.index},
					{"timestamp", value.stackMem.timeprofiler.timestamp},
					{"peakTimestamp", value.stackMem.timeprofiler.peakTimesteamp},
					{"peakMemory", value.stackMem.timeprofiler.peakMemory},
					{"peakIndex", value.stackMem.timeprofiler.peakIndex},
					{"linearIndex", value.stackMem.timeprofiler.linearIndex},
				}
			},
		}
	};
}

/**********************************************************/
void from_json(const nlohmann::json & json, Thread & value)
{
	//checks
	assert(json.contains("stackMem"));
	assert(json.contains("cntMemOps"));
	assert(json.contains("stats"));

	//stats
	const nlohmann::json & stackMem = json.at("stackMem");
	assert(stackMem.contains("stack"));
	assert(stackMem.contains("mem"));
	assert(stackMem.contains("total"));
	assert(stackMem.contains("timeprofiler"));
	stackMem.at("stack").get_to(value.stackMem.stack);
	stackMem.at("mem").get_to(value.stackMem.mem);
	stackMem.at("total").get_to(value.stackMem.total);

	//timeprofiler
	const nlohmann::json & timeprofiler = stackMem.at("timeprofiler");
	assert(timeprofiler.contains("min"));
	assert(timeprofiler.contains("max"));
	assert(timeprofiler.contains("index"));
	assert(timeprofiler.contains("timestamp"));
	assert(timeprofiler.contains("peakTimesteamp"));
	assert(timeprofiler.contains("peakMemory"));
	assert(timeprofiler.contains("peakIndex"));
	assert(timeprofiler.contains("linearIndex"));
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
	assert(stats.contains("malloc"));
	assert(stats.contains("posix_memalign"));
	assert(stats.contains("aligned_alloc"));
	assert(stats.contains("memalign"));
	assert(stats.contains("valloc"));
	assert(stats.contains("pvalloc"));
	assert(stats.contains("free"));
	assert(stats.contains("calloc"));
	assert(stats.contains("realloc"));
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
