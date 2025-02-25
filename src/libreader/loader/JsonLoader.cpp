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
#include "JsonLoader.hpp"

/**********************************************************/
using namespace MALTReader;
using namespace MALTFormat;
using json = nlohmann::json;

/**********************************************************/
JsonLoader::JsonLoader(const std::string & fname)
{
	this->loadFile(fname);
}

/**********************************************************/
JsonLoader::~JsonLoader(void)
{

}

/**********************************************************/
void JsonLoader::loadFile(const std::string & fname)
{
	//open stream
	std::ifstream f(fname);

	//load it
	printf("Parsing JSON...\n");
	json data = json::parse(f);
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
			JsonLoader::load(profile.run, json["run"]);
			#pragma omp task
			JsonLoader::load(profile.config, json["config"]);
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
void JsonLoader::load(MALTFormat::Run & run, const nlohmann::json & json)
{
	//checks
	assert(json.contains("formatVersion"));
	assert(json.contains("tool"));
	assert(json.contains("runtime"));
	assert(json.contains("allocator"));
	assert(json.contains("exe"));
	assert(json.contains("command"));
	assert(json.contains("hostname"));
	//assert(json.size() == 7);

	//load
	json.at("formatVersion").get_to(run.formatVersion);
	json.at("tool").get_to(run.tool);
	json.at("runtime").get_to(run.runtime);
	json.at("allocator").get_to(run.allocator);
	json.at("exe").get_to(run.exe);
	json.at("command").get_to(run.command);
	json.at("hostname").get_to(run.hostname);
}

/**********************************************************/
void JsonLoader::load(MALTFormat::Config & config, const nlohmann::json & json)
{
	//checks
	assert(json.contains("time"));

	//time
	nlohmann::json jsonTime = json.at("time");
	assert(jsonTime.contains("enabled"));
	assert(jsonTime.contains("points"));
	assert(jsonTime.contains("linear"));
	jsonTime.at("enabled").get_to(config.time.enabled);
	jsonTime.at("points").get_to(config.time.points);
	jsonTime.at("linear").get_to(config.time.linear);

	//stack
	nlohmann::json jsonStack = json.at("stack");
	assert(jsonStack.contains("enabled"));
	assert(jsonStack.contains("mode"));
	assert(jsonStack.contains("resolve"));
	assert(jsonStack.contains("libunwind"));
	assert(jsonStack.contains("stackSkip"));
	assert(jsonStack.contains("addr2lineBucket"));
	assert(jsonStack.contains("addr2lineThreads"));
	jsonStack.at("enabled").get_to(config.stack.enabled);
	jsonStack.at("mode").get_to(config.stack.mode);
	jsonStack.at("resolve").get_to(config.stack.resolve);
	jsonStack.at("libunwind").get_to(config.stack.libunwind);
	jsonStack.at("stackSkip").get_to(config.stack.stackSkip);
	jsonStack.at("addr2lineBucket").get_to(config.stack.addr2lineBucket);
	jsonStack.at("addr2lineThreads").get_to(config.stack.addr2lineThreads);

	//python
	nlohmann::json jsonPython = json.at("python");
	assert(jsonPython.contains("instru"));
	assert(jsonPython.contains("mix"));
	assert(jsonPython.contains("stack"));
	jsonPython.at("instru").get_to(config.python.instru);
	jsonPython.at("mix").get_to(config.python.mix);
	jsonPython.at("stack").get_to(config.python.stack);

	//output
	nlohmann::json jsonOutput = json.at("output");
	assert(jsonOutput.contains("callgrind"));
	assert(jsonOutput.contains("dumpConfig"));
	assert(jsonOutput.contains("index"));
	assert(jsonOutput.contains("json"));
	assert(jsonOutput.contains("lua"));
	assert(jsonOutput.contains("name"));
	assert(jsonOutput.contains("verbosity"));
	assert(jsonOutput.contains("stackTree"));
	assert(jsonOutput.contains("loopSuppress"));
	jsonOutput.at("callgrind").get_to(config.output.callgrind);
	jsonOutput.at("dumpConfig").get_to(config.output.dumpConfig);
	jsonOutput.at("index").get_to(config.output.index);
	jsonOutput.at("json").get_to(config.output.json);
	jsonOutput.at("lua").get_to(config.output.lua);
	jsonOutput.at("name").get_to(config.output.name);
	jsonOutput.at("verbosity").get_to(config.output.verbosity);
	jsonOutput.at("stackTree").get_to(config.output.stackTree);
	jsonOutput.at("loopSuppress").get_to(config.output.loopSuppress);

	//maxStack
	nlohmann::json jsonMaxSatck = json.at("maxStack");
	assert(jsonMaxSatck.contains("enabled"));
	jsonMaxSatck.at("enabled").get_to(config.maxStack.enabled);

	//distr
	nlohmann::json jsonDistr = json.at("distr");
	assert(jsonDistr.contains("allocSize"));
	assert(jsonDistr.contains("reallocJump"));
	jsonDistr.at("allocSize").get_to(config.distr.allocSize);
	jsonDistr.at("reallocJump").get_to(config.distr.reallocJump);

	//maxStack
	nlohmann::json jsonInfo = json.at("info");
	assert(jsonInfo.contains("hidden"));
	jsonInfo.at("hidden").get_to(config.info.hidden);

	//maxStack
	nlohmann::json jsonFilter = json.at("filter");
	assert(jsonFilter.contains("exe"));
	assert(jsonFilter.contains("childs"));
	assert(jsonFilter.contains("enabled"));
	jsonFilter.at("exe").get_to(config.filter.exe);
	jsonFilter.at("childs").get_to(config.filter.childs);
	jsonFilter.at("enabled").get_to(config.filter.enabled);

	//maxStack
	nlohmann::json jsonDump = json.at("dump");
	assert(jsonDump.contains("onSignal"));
	assert(jsonDump.contains("afterSeconds"));
	jsonDump.at("onSignal").get_to(config.dump.onSignal);
	jsonDump.at("afterSeconds").get_to(config.dump.afterSeconds);
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
	size_t fileId = 0;
	json["file"].get_to(fileId);
	size_t binaryId = 0;
	json["binary"].get_to(binaryId);
	size_t functionId = 0;
	json["function"].get_to(functionId);

	//fill
	assert(fileId < strings.size());
	instrInfos.file = &strings[fileId];
	assert(binaryId < strings.size());
	instrInfos.binary = &strings[binaryId];
	assert(functionId < strings.size());
	instrInfos.function = &strings[functionId];
	json["line"].get_to(instrInfos.line);
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
