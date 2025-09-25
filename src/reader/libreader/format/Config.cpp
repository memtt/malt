/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 09/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/reader/libreader/format/Config.cpp
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
				{"sampling", config.stack.sampling},
				{"samplingBw", config.stack.samplingBw},
			}
		},
		{
			"python", {
				{"instru", config.python.instru},
				{"mix", config.python.mix},
				{"stack", config.python.stack},
				{"obj", config.python.obj},
				{"mem", config.python.mem},
				{"raw", config.python.raw},
				{"hideImports", config.python.hideImports},
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
				{"ranks", config.filter.ranks},
			}
		},
		{
			"dump", {
				{"onSignal", config.dump.onSignal},
				{"afterSeconds", config.dump.afterSeconds},
				{"onSysFullAt", config.dump.onSysFullAt},
				{"onAppUsingRss", config.dump.onAppUsingRss},
				{"onAppUsingVirt", config.dump.onAppUsingVirt},
				{"onAppUsingReq", config.dump.onAppUsingReq},
				{"onThreadStackUsing", config.dump.onThreadStackUsing},
				{"onAllocCount", config.dump.onAllocCount},
				{"watchDog", config.dump.watchDog},
			}
		},
		{
			"tools", {
				{"nm", config.tools.nm},
				{"nmMaxSize", config.tools.nmMaxSize},
			}
		}
	};
}

/**********************************************************/
void from_json(const JsonIn & json, Config & config)
{
	//checks
	assert(jsContains(json, "time"));

	//time
	JsonIn jsonTime = json.at("time");
	assert(jsContains(jsonTime, "enabled"));
	assert(jsContains(jsonTime, "points"));
	assert(jsContains(jsonTime, "linear"));
	jsonTime.at("enabled").get_to(config.time.enabled);
	jsonTime.at("points").get_to(config.time.points);
	jsonTime.at("linear").get_to(config.time.linear);

	//stack
	JsonIn jsonStack = json.at("stack");
	assert(jsContains(jsonStack, "enabled"));
	assert(jsContains(jsonStack, "mode"));
	assert(jsContains(jsonStack, "resolve"));
	assert(jsContains(jsonStack, "libunwind"));
	assert(jsContains(jsonStack, "stackSkip"));
	assert(jsContains(jsonStack, "addr2lineBucket"));
	assert(jsContains(jsonStack, "addr2lineThreads"));
	assert(jsContains(jsonStack, "sampling"));
	assert(jsContains(jsonStack, "samplingBw"));
	jsonStack.at("enabled").get_to(config.stack.enabled);
	jsonStack.at("mode").get_to(config.stack.mode);
	jsonStack.at("resolve").get_to(config.stack.resolve);
	jsonStack.at("libunwind").get_to(config.stack.libunwind);
	jsonStack.at("stackSkip").get_to(config.stack.stackSkip);
	jsonStack.at("addr2lineBucket").get_to(config.stack.addr2lineBucket);
	jsonStack.at("addr2lineThreads").get_to(config.stack.addr2lineThreads);
	jsonStack.at("sampling").get_to(config.stack.sampling);
	jsonStack.at("samplingBw").get_to(config.stack.samplingBw);

	//python
	JsonIn jsonPython = json.at("python");
	assert(jsContains(jsonPython, "instru"));
	assert(jsContains(jsonPython, "mix"));
	assert(jsContains(jsonPython, "stack"));
	assert(jsContains(jsonPython, "obj"));
	assert(jsContains(jsonPython, "mem"));
	assert(jsContains(jsonPython, "raw"));
	assert(jsContains(jsonPython, "hideImports"));
	jsonPython.at("instru").get_to(config.python.instru);
	jsonPython.at("mix").get_to(config.python.mix);
	jsonPython.at("stack").get_to(config.python.stack);
	jsonPython.at("obj").get_to(config.python.obj);
	jsonPython.at("mem").get_to(config.python.mem);
	jsonPython.at("raw").get_to(config.python.raw);
	jsonPython.at("hideImports").get_to(config.python.hideImports);

	//output
	JsonIn jsonOutput = json.at("output");
	assert(jsContains(jsonOutput, "callgrind"));
	assert(jsContains(jsonOutput, "dumpConfig"));
	assert(jsContains(jsonOutput, "index"));
	assert(jsContains(jsonOutput, "json"));
	assert(jsContains(jsonOutput, "lua"));
	assert(jsContains(jsonOutput, "name"));
	assert(jsContains(jsonOutput, "verbosity"));
	assert(jsContains(jsonOutput, "stackTree"));
	assert(jsContains(jsonOutput, "loopSuppress"));
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
	JsonIn jsonMaxSatck = json.at("maxStack");
	assert(jsContains(jsonMaxSatck, "enabled"));
	jsonMaxSatck.at("enabled").get_to(config.maxStack.enabled);

	//distr
	JsonIn jsonDistr = json.at("distr");
	assert(jsContains(jsonDistr, "allocSize"));
	assert(jsContains(jsonDistr, "reallocJump"));
	jsonDistr.at("allocSize").get_to(config.distr.allocSize);
	jsonDistr.at("reallocJump").get_to(config.distr.reallocJump);

	//maxStack
	JsonIn jsonInfo = json.at("info");
	assert(jsContains(jsonInfo, "hidden"));
	jsonInfo.at("hidden").get_to(config.info.hidden);

	//maxStack
	JsonIn jsonFilter = json.at("filter");
	assert(jsContains(jsonFilter, "exe"));
	assert(jsContains(jsonFilter, "childs"));
	assert(jsContains(jsonFilter, "enabled"));
	assert(jsContains(jsonFilter, "ranks"));
	jsonFilter.at("exe").get_to(config.filter.exe);
	jsonFilter.at("childs").get_to(config.filter.childs);
	jsonFilter.at("enabled").get_to(config.filter.enabled);
	jsonFilter.at("ranks").get_to(config.filter.ranks);

	//maxStack
	JsonIn jsonDump = json.at("dump");
	assert(jsContains(jsonDump, "onSignal"));
	assert(jsContains(jsonDump, "afterSeconds"));
	jsonDump.at("onSignal").get_to(config.dump.onSignal);
	jsonDump.at("afterSeconds").get_to(config.dump.afterSeconds);
	jsonDump.at("onSysFullAt").get_to(config.dump.onSysFullAt);
	jsonDump.at("onAppUsingRss").get_to(config.dump.onAppUsingRss);
	jsonDump.at("onAppUsingVirt").get_to(config.dump.onAppUsingVirt);
	jsonDump.at("onAppUsingReq").get_to(config.dump.onAppUsingReq);
	jsonDump.at("onThreadStackUsing").get_to(config.dump.onThreadStackUsing);
	jsonDump.at("onAllocCount").get_to(config.dump.onAllocCount);
	jsonDump.at("watchDog").get_to(config.dump.watchDog);

	//maxStack
	JsonIn jsonTools = json.at("tools");
	assert(jsContains(jsonTools, "nm"));
	assert(jsContains(jsonTools, "nmMaxSize"));
	jsonTools.at("nm").get_to(config.tools.nm);
	jsonTools.at("nmMaxSize").get_to(config.tools.nmMaxSize);
}

}
