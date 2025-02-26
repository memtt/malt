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
#include "Config.hpp"

/**********************************************************/
namespace MALTFormat
{

/**********************************************************/
void to_json(nlohmann::json & json, const Config & config){
	json = nlohmann::json{
		{
			"time", {
				{"enabled", config.time.enabled},
				{"points", config.time.points},
				{"linear", config.time.linear}
			}
		},
		{
			"stack", {
				{"enabled", config.stack.enabled},
				{"mode", config.stack.mode},
				{"resolve", config.stack.resolve},
				{"libunwind", config.stack.libunwind},
				{"stackSkip", config.stack.stackSkip},
				{"addr2lineBucket", config.stack.addr2lineBucket},
				{"addr2lineThreads", config.stack.addr2lineThreads},
			}
		},
		{
			"python", {
				{"instru", config.python.instru},
				{"mix", config.python.mix},
				{"stack", config.python.stack},
			}
		},
		{
			"output", {
				{"callgrind", config.output.callgrind},
				{"dumpConfig", config.output.dumpConfig},
				{"index", config.output.index},
				{"json", config.output.json},
				{"lua", config.output.lua},
				{"name", config.output.name},
				{"verbosity", config.output.verbosity},
				{"stackTree", config.output.stackTree},
				{"loopSuppress", config.output.loopSuppress},
			}
		},
		{
			"maxStack", {
				{"enabled", config.maxStack.enabled},
			}
		},
		{
			"distr", {
				{"allocSize", config.distr.allocSize},
				{"reallocJump", config.distr.reallocJump},
			}
		},
		{
			"info", {
				{"hidden", config.info.hidden},
			}
		},
		{
			"filter", {
				{"exe", config.filter.exe},
				{"childs", config.filter.childs},
				{"enabled", config.filter.enabled},
			}
		},
		{
			"dump", {
				{"onSignal", config.dump.onSignal},
				{"afterSeconds", config.dump.afterSeconds}
			}
		},
	};
}

/**********************************************************/
void from_json(const nlohmann::json & json, Config & config)
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

}
