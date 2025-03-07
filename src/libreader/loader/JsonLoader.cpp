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
	json data = json::parse(f);
	this->load(this->profile, data);
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
		#pragma omp task
		JsonLoader::load(profile.run, json["run"]);
		#pragma omp task
		JsonLoader::load(profile.config, json["config"]);
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
	nlohmann::json jsonTime = json.at("times");
	assert(jsonTime.contains("enabled"));
	assert(jsonTime.contains("points"));
	assert(jsonTime.contains("linear"));
	jsonTime.at("enabled").get_to(config.time.enabled);
	jsonTime.at("points").get_to(config.time.points);
	jsonTime.at("linear").get_to(config.time.linear);

	//stack
	nlohmann::json jsonStack = json.at("stack");
	assert(jsonStack.contains("enabled"));
	assert(jsonStack.contains("backtrace"));
	assert(jsonStack.contains("resolve"));
	assert(jsonStack.contains("libunwind"));
	assert(jsonStack.contains("stackSkip"));
	assert(jsonStack.contains("addr2lineBucket"));
	assert(jsonStack.contains("addr2lineThreads"));
	jsonStack.at("enabled").get_to(config.stack.enabled);
	jsonStack.at("backtrace").get_to(config.stack.backtrace);
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
	assert(jsonDistr.contains("enabled"));
	assert(jsonDistr.contains("enabled"));
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
	assert(jsonFilter.contains("hidden"));
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
