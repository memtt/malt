/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 01/2026
*    LICENSE  : CeCILL-C
*    FILE     : src/reader/libreader/format/Config.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
*    AUTHOR   : Sébastien Valat - 2026
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
				{"skip", config.stack.skip},
			}
		},
		{
			"addr2line", {
				{"bucket", config.addr2line.bucket},
				{"threads", config.addr2line.threads},
				{"huge", config.addr2line.huge},
			}
		},
		{
			"sampling", {
				{"enabled", config.sampling.enabled},
				{"volume", config.sampling.volume},
				{"count", config.sampling.count},
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
				{"hide-imports", config.python.hideImports},
				{"mode", config.python.mode},
			}
		},
		{
			"c", {
				{"gpu-malloc", config.c.gpuMalloc},
				{"malloc", config.c.malloc},
				{"mmap", config.c.mmap},
			}
		},
		{
			"output", {
				{"callgrind", config.output.callgrind},
				{"config", config.output.dumpConfig},
				{"indent", config.output.indent},
				{"json", config.output.json},
				{"lua", config.output.lua},
				{"name", config.output.name},
				{"verbosity", config.output.verbosity},
				{"stack-tree", config.output.stackTree},
				{"loop-suppress", config.output.loopSuppress},
			}
		},
		{
			"max-stack", {
				{"enabled", config.maxStack.enabled},
			}
		},
		{
			"trace", {
				{ "enabled", config.trace.enabled},
			}
		},
		{
			"distr", {
				{"alloc-size", config.distr.allocSize},
				{"realloc-jump", config.distr.reallocJump},
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
				{"on-signal", config.dump.onSignal},
				{"after-seconds", config.dump.afterSeconds},
				{"on-sys-full-at", config.dump.onSysFullAt},
				{"on-app-using-rss", config.dump.onAppUsingRss},
				{"on-app-using-virt", config.dump.onAppUsingVirt},
				{"on-app-using-req", config.dump.onAppUsingReq},
				{"on-thread-stack-using", config.dump.onThreadStackUsing},
				{"on-alloc-count", config.dump.onAllocCount},
				{"watch-dog", config.dump.watchDog},
			}
		},
		{
			"tools", {
				{"nm", config.tools.nm},
				{"nm-max-size", config.tools.nmMaxSize},
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
	assert(jsContains(jsonStack, "skip"));
	jsonStack.at("enabled").get_to(config.stack.enabled);
	jsonStack.at("mode").get_to(config.stack.mode);
	jsonStack.at("resolve").get_to(config.stack.resolve);
	jsonStack.at("libunwind").get_to(config.stack.libunwind);
	jsonStack.at("skip").get_to(config.stack.skip);

	//addr2line
	JsonIn jsonAddr2line = json.at("addr2line");
	assert(jsContains(jsonAddr2line, "bucket"));
	assert(jsContains(jsonAddr2line, "threads"));
	assert(jsContains(jsonAddr2line, "huge"));
	jsonAddr2line.at("bucket").get_to(config.addr2line.bucket);
	jsonAddr2line.at("threads").get_to(config.addr2line.threads);
	jsonAddr2line.at("huge").get_to(config.addr2line.huge);

	//sampling
	JsonIn jsonSmpling = json.at("sampling");
	assert(jsContains(jsonSmpling, "enabled"));
	assert(jsContains(jsonSmpling, "volume"));
	assert(jsContains(jsonSmpling, "count"));
	jsonSmpling.at("enabled").get_to(config.sampling.enabled);
	jsonSmpling.at("volume").get_to(config.sampling.volume);
	jsonSmpling.at("count").get_to(config.sampling.count);

	//python
	JsonIn jsonPython = json.at("python");
	assert(jsContains(jsonPython, "instru"));
	assert(jsContains(jsonPython, "mix"));
	assert(jsContains(jsonPython, "stack"));
	assert(jsContains(jsonPython, "obj"));
	assert(jsContains(jsonPython, "mem"));
	assert(jsContains(jsonPython, "raw"));
	assert(jsContains(jsonPython, "hide-imports"));
	jsonPython.at("instru").get_to(config.python.instru);
	jsonPython.at("mix").get_to(config.python.mix);
	jsonPython.at("stack").get_to(config.python.stack);
	jsonPython.at("obj").get_to(config.python.obj);
	jsonPython.at("mem").get_to(config.python.mem);
	jsonPython.at("raw").get_to(config.python.raw);
	jsonPython.at("hide-imports").get_to(config.python.hideImports);
	jsonPython.at("mode").get_to(config.python.mode);

	//python
	JsonIn jsonC = json.at("c");
	assert(jsContains(jsonC, "gpu-malloc"));
	assert(jsContains(jsonC, "malloc"));
	assert(jsContains(jsonC, "mmap"));
	jsonC.at("gpu-malloc").get_to(config.c.gpuMalloc);
	jsonC.at("malloc").get_to(config.c.malloc);
	jsonC.at("mmap").get_to(config.c.mmap);

	//output
	JsonIn jsonOutput = json.at("output");
	assert(jsContains(jsonOutput, "callgrind"));
	assert(jsContains(jsonOutput, "config"));
	assert(jsContains(jsonOutput, "json"));
	assert(jsContains(jsonOutput, "lua"));
	assert(jsContains(jsonOutput, "name"));
	assert(jsContains(jsonOutput, "verbosity"));
	assert(jsContains(jsonOutput, "config"));
	assert(jsContains(jsonOutput, "indent"));
	assert(jsContains(jsonOutput, "stack-tree"));
	assert(jsContains(jsonOutput, "loop-suppress"));
	jsonOutput.at("callgrind").get_to(config.output.callgrind);
	jsonOutput.at("config").get_to(config.output.dumpConfig);
	jsonOutput.at("json").get_to(config.output.json);
	jsonOutput.at("lua").get_to(config.output.lua);
	jsonOutput.at("name").get_to(config.output.name);
	jsonOutput.at("verbosity").get_to(config.output.verbosity);
	jsonOutput.at("stack-tree").get_to(config.output.stackTree);
	jsonOutput.at("loop-suppress").get_to(config.output.loopSuppress);
	jsonOutput.at("indent").get_to(config.output.indent);

	//maxStack
	JsonIn jsonMaxSatck = json.at("max-stack");
	assert(jsContains(jsonMaxSatck, "enabled"));
	jsonMaxSatck.at("enabled").get_to(config.maxStack.enabled);

	//distr
	JsonIn jsonDistr = json.at("distr");
	assert(jsContains(jsonDistr, "alloc-size"));
	assert(jsContains(jsonDistr, "realloc-jump"));
	jsonDistr.at("alloc-size").get_to(config.distr.allocSize);
	jsonDistr.at("realloc-jump").get_to(config.distr.reallocJump);

	//maxStack
	JsonIn jsonInfo = json.at("info");
	assert(jsContains(jsonInfo, "hidden"));
	jsonInfo.at("hidden").get_to(config.info.hidden);

	//time
	JsonIn jsonTrace = json.at("trace");
	assert(jsContains(jsonTrace, "enabled"));
	jsonTrace.at("enabled").get_to(config.trace.enabled);

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
	assert(jsContains(jsonDump, "on-signal"));
	assert(jsContains(jsonDump, "after-seconds"));
	jsonDump.at("on-signal").get_to(config.dump.onSignal);
	jsonDump.at("after-seconds").get_to(config.dump.afterSeconds);
	jsonDump.at("on-sys-full-at").get_to(config.dump.onSysFullAt);
	jsonDump.at("on-app-using-rss").get_to(config.dump.onAppUsingRss);
	jsonDump.at("on-app-using-virt").get_to(config.dump.onAppUsingVirt);
	jsonDump.at("on-app-using-req").get_to(config.dump.onAppUsingReq);
	jsonDump.at("on-thread-stack-using").get_to(config.dump.onThreadStackUsing);
	jsonDump.at("on-alloc-count").get_to(config.dump.onAllocCount);
	jsonDump.at("watch-dog").get_to(config.dump.watchDog);

	//maxStack
	JsonIn jsonTools = json.at("tools");
	assert(jsContains(jsonTools, "nm"));
	assert(jsContains(jsonTools, "nm-max-size"));
	jsonTools.at("nm").get_to(config.tools.nm);
	jsonTools.at("nm-max-size").get_to(config.tools.nmMaxSize);
}

}
