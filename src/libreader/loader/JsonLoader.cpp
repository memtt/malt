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
//keep debug for error handling for now
#undef NDEBUG
#include <cassert>
//to parse json
#include <fstream>
//local
#include "FileReader.hpp"
#include "JsonLoader.hpp"

/**********************************************************/
using namespace MALTFormat;
using json = nlohmann::json;

/**********************************************************/
namespace MALTReader
{

/**********************************************************/
JsonLoader::JsonLoader(const std::string & fname, bool progressBar)
{
	this->loadFile(fname, progressBar);
}

/**********************************************************/
JsonLoader::~JsonLoader(void)
{

}

/**********************************************************/
void JsonLoader::loadFile(const std::string & fname, bool progressBar)
{
	//open stream
	FileReader reader(fname, progressBar);

	//load it
	printf("Parsing JSON...\n");
	json data = json::parse(begin(reader), end(reader));
	printf("Extracting infos...\n");
	this->load(this->profile, data);
	printf("Done.\n");
}

/**********************************************************/
MaltProfile & JsonLoader::getProfile(void)
{
	return this->profile;
}

/**********************************************************/
const MaltProfile & JsonLoader::getProfile(void) const
{
	return this->profile;
}

/**********************************************************/
void JsonLoader::load(MALTFormat::MaltProfile & profile, const nlohmann::json & json)
{
	//check keys
	assert(json.contains("run"));
	assert(json.contains("config"));
	assert(json.contains("stacks"));
	assert(json.contains("sites"));
	assert(json.contains("timeline"));
	assert(json.contains("scatter"));
	assert(json.contains("threads"));
	assert(json.contains("globals"));
	assert(json.contains("leaks"));
	//assert(json.size() == 9);

	//load
	#pragma omp parallel
	{
		#pragma omp single
		{
			#pragma omp task
			json["run"].get_to(profile.run);
			#pragma omp task
			json["config"].get_to(profile.config);
			#pragma omp task
			JsonLoader::load(profile.stacks, json["stacks"]);
			#pragma omp task
			JsonLoader::load(profile.sites, json["sites"]);
			#pragma omp task
			JsonLoader::load(profile.timeline, json["timeline"]);
			#pragma omp task
			JsonLoader::load(profile.scatter, json["scatter"]);
			#pragma omp task
			JsonLoader::load(profile.threads, json["threads"]);
			#pragma omp task
			JsonLoader::load(profile.globals, json["globals"]);
			#pragma omp task
			JsonLoader::load(profile.leaks, json["leaks"]);
		}
	}
}

/**********************************************************/
void JsonLoader::load(MALTFormat::Stacks & stacks, const nlohmann::json & json)
{
	//check
	assert(json.contains("stats"));
	assert(json.contains("count"));

	//get it
	const nlohmann::json & stats = json["stats"];
	assert(stats.is_array());
	stacks.stats.reserve(stats.size());
	for (const auto & it : stats) {
		auto & stat = stacks.stats.emplace_back();
		#pragma omp task
		JsonLoader::load(stat, it);
	}
}

/**********************************************************/
void JsonLoader::load(MALTFormat::StackStats & stats, const nlohmann::json & json)
{
	//check
	assert(json.contains("stack"));
	assert(json.contains("stackId"));
	assert(json.contains("infos"));

	//stack
	const nlohmann::json & stack = json["stack"];
	assert(stack.is_array());
	stats.stack.reserve(stack.size());
	for (const auto & it : stack)
		JsonLoader::load(stats.stack.emplace_back(), it);

	//load
	JsonLoader::load(stats.stackId, json["stackId"]);
	JsonLoader::load(stats.infos, json["infos"]);
}

/**********************************************************/
void JsonLoader::load(MALTFormat::StackInfos & stackInfos, const nlohmann::json & json)
{
	//check
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
	json["countZeros"].get_to(stackInfos.countZeros);
	json["maxAliveReq"].get_to(stackInfos.maxAliveReq);
	JsonLoader::load(stackInfos.alloc, json["alloc"]);
	JsonLoader::load(stackInfos.free, json["free"]);
	JsonLoader::load(stackInfos.lifetime, json["lifetime"]);
	json["globalPeak"].get_to(stackInfos.globalPeak);
	json["reallocCount"].get_to(stackInfos.reallocCount);
	json["reallocSumDelta"].get_to(stackInfos.reallocSumDelta);
}

/**********************************************************/
void JsonLoader::load(MALTFormat::LangAddress & address, const nlohmann::json & json)
{
	//check
	assert(json.is_string());

	//parse
	std::string value = json;

	//cases
	void * ptr = nullptr;
	if (sscanf(value.c_str(), "PY-0x%p", &ptr) == 1) {
		address.lang = LANG_PYTHON;
		address.address = ptr;
	} else if (sscanf(value.c_str(), "0x%p", &ptr) == 1) {
		address.lang = LANG_C;
		address.address = ptr;
	} else {
		assert(false);
	}
}

/**********************************************************/
void JsonLoader::load(MALTFormat::CountMinMaxSum & minMaxSum, const nlohmann::json & json)
{
	//check
	assert(json.contains("count"));
	assert(json.contains("min"));
	assert(json.contains("max"));
	assert(json.contains("sum"));

	//load
	json["count"].get_to(minMaxSum.count);
	json["min"].get_to(minMaxSum.min);
	json["max"].get_to(minMaxSum.max);
	json["sum"].get_to(minMaxSum.sum);
}

/**********************************************************/
void JsonLoader::load(void* & ptr, const nlohmann::json & json)
{
	//check
	assert(json.is_string());

	//convert
	std::string value = json;

	//parse
	if (sscanf(value.c_str(), "0x%p", &ptr) == 0)
		assert(false);
}

/**********************************************************/
void JsonLoader::load(ProcMapEntry & procMapEntry, const nlohmann::json & json)
{
	//check
	assert(json.contains("lower"));
	assert(json.contains("upper"));
	assert(json.contains("offset"));
	assert(json.contains("aslrOffset"));
	assert(json.contains("file"));

	//load
	JsonLoader::load(procMapEntry.lower, json["lower"]);
	JsonLoader::load(procMapEntry.upper, json["upper"]);
	json["offset"].get_to(procMapEntry.offset);
	json["aslrOffset"].get_to(procMapEntry.aslrOffset);
	json["file"].get_to(procMapEntry.file);
};

/**********************************************************/
void JsonLoader::load(InstructionInfos & instrInfos, const nlohmann::json & json, const std::vector<std::string> & strings)
{
	//check
	assert(json.contains("file"));
	assert(json.contains("binary"));
	assert(json.contains("function"));
	assert(json.contains("line"));

	//extract
	json.at("file").get_to(instrInfos.file);
	json.at("binary").get_to(instrInfos.binary);
	json.at("function").get_to(instrInfos.function);
};

/**********************************************************/
void JsonLoader::load(Sites & sites, const nlohmann::json & json)
{
	/*
	std::vector<ProcMapEntry> map;
	std::vector<std::string> strings;
	std::map<void*, InstructionInfos> instr;
	*/
};

/**********************************************************/
void JsonLoader::load(CallSite & callSite, const nlohmann::json & json)
{
	/*
	LangAddress orig;
	const InstructionInfos * translated;
	*/
};

/**********************************************************/
void JsonLoader::load(TimelineInfos & timelineInfos, const nlohmann::json & json)
{
	/*
	CyclesTime start;
	std::vector<std::string> fields;
	CyclesDuration perPoints;
	std::vector<size_t> peak;
	std::vector< std::vector<size_t> > values;
	std::vector<CallSite> callsite;
	*/
};

/**********************************************************/
void JsonLoader::load(Timeline & timeline, const nlohmann::json & json)
{
	/*
	TimelineInfos memoryTimeline;
	TimelineInfos systemTimeline;
	TimelineInfos memoryBandwidth;
	*/
};

/**********************************************************/
void JsonLoader::load(MALTFormat::ScatterPoint& scatterPoint, const nlohmann::json & json)
{
	/*
	size_t x;
	size_t y;
	size_t v;
	*/
}

/**********************************************************/
void JsonLoader::load(MALTFormat::ScatterChart& scatterChart, const nlohmann::json & json)
{
	/*
	struct {
		struct {
			size_t max;
			size_t size;
			bool log;
		} xAxis;
		struct {
			size_t max;
			size_t size;
			bool log;
		} yAxis;
	} infos;
	std::vector<ScatterPoint> points;
	*/
}

/**********************************************************/
void JsonLoader::load(MALTFormat::Scatter& scatter, const nlohmann::json & json)
{
	/*
	ScatterChart sizeOverTime;
	ScatterChart lifetimeOverSize;
	*/
}

/**********************************************************/
void JsonLoader::load(MALTFormat::Threads& threads, const nlohmann::json & json)
{
	/*
	struct {
		size_t size;
		std::vector<LangAddress> stack;
		std::vector<size_t> mem;
		size_t total;
		struct {
			std::vector<size_t> min;
			std::vector<size_t> max;
			std::vector<size_t> index;
			std::vector<CyclesTime> timestamp;
			CyclesTime peakTimestamp;
			size_t peakMemory;
			size_t peakIndex;
			bool linearIndex;
		} timeprofiler;
	} stackMem;
	size_t cntMemOps;
	struct {
		ThreadsFuncStats malloc;
		ThreadsFuncStats posix_memalign;
		ThreadsFuncStats aligned_alloc;
		ThreadsFuncStats memalign;
		ThreadsFuncStats valloc;
		ThreadsFuncStats pvalloc;
		ThreadsFuncStats free;
		ThreadsFuncStats calloc;
		ThreadsFuncStats realloc;
	} stats;
	*/
}

/**********************************************************/
void JsonLoader::load(MALTFormat::ThreadsFuncStats& threadsFuncStats, const nlohmann::json & json)
{
	/*
	size_t count;
	size_t sum;
	CyclesDuration time;
	*/
}

/**********************************************************/
void JsonLoader::load(MALTFormat::Globals& globals, const nlohmann::json & json)
{
	/*
	CyclesDuration ticksPerSecond;
	size_t totalMemory;
	size_t freeMemoryAtStart;
	size_t cachedMemoryAtStart;
	size_t maxThreadCount;
	*/
}

/**********************************************************/
void JsonLoader::load(MALTFormat::Leaks& leaks, const nlohmann::json & json)
{
	/*array of leak*/
}

/**********************************************************/
void JsonLoader::load(MALTFormat::Leak& leak, const nlohmann::json & json)
{
	/*
	std::vector<LangAddress> stack;
	size_t count;
	size_t memory;
	*/
}

}
