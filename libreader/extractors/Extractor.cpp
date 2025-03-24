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
#include <regex>
#include "ExtractorHelpers.hpp"
#include "Extractor.hpp"

/**********************************************************/
using namespace MALTFormat;

/**********************************************************/
namespace MALTReader
{

/**********************************************************/
Extractor::Extractor(const MALTFormat::MaltProfile & profile)
	:profile(profile)
{
	this->buildTranslation();
}

/**********************************************************/
Extractor::~Extractor(void)
{
}

/**********************************************************/
void Extractor::buildTranslation(void)
{
	//vars
	const StackStats & stats = this->profile.stacks.stats;
	const SitesInstrMap & instrs  = this->profile.sites.instr;
	const std::vector<std::string> & strings = this->profile.sites.strings;

	//loop on all
	for (const auto & statEntry : stats) {
		for (const LangAddress & addr : statEntry.stack) {
			//get site
			const auto & it = instrs.find(addr);
			assert(it != instrs.end());
			const InstructionInfos & instrInfos = it->second;

			//build ref
			InstructionInfosStrRef ref;
			ref.binary = &this->getString(instrInfos.binary);
			ref.function = &this->getString(instrInfos.function);
			ref.file = &this->getString(instrInfos.file);
			ref.line = instrInfos.line;
			ref.origin = addr;

			//store
			this->addrTranslation[addr] = ref;
		}
	}
}

/**********************************************************/
FlatProfileVector Extractor::getFlatProfile(const LocaltionMappingFunc & mapping,const LocaltionFilterFunc & filter) const
{
	//vars
	FlatProfileMap resultGeneral;
	const StackStats & stats = this->profile.stacks.stats;

	//loop on all
	#pragma omp parallel
	{
		//local result to threading
		FlatProfileMap result;

		//loop with threads
		#pragma omp for
		for (size_t i = 0 ; i < stats.size() ; i++) {
			//extract some short refs
			const StackStat & statEntry = stats[i];
			const StackInfos & infos = statEntry.infos;
			const Stack & stack = statEntry.stack;

			//skip C++ operators
			size_t skip = 0;
			while (skip < stack.size() && ExtractorHelpers::isAllocFunction(*addrTranslation.at(stack[skip]).function)) skip++;
			if (skip >= stack.size())
			{
				std::cerr << "Warning : get call stacks with only allocation function ??? : " << std::endl;
				//TODO make serialization of stacks
				for (const auto it : stack) {
					InstructionInfosStrRef infosRef = addrTranslation.at(stack[skip]);
					std::cerr << "           - " << *infosRef.file << ":" << infosRef.line << " (" << *infosRef.function << ")" << std::endl;
				}
				//TODO print infos
				continue;
			}

			//update internal values
			LangAddress cur = stack[skip];
			if (filter(this->addrTranslation.at(cur),infos) == true)
				this->mergeStackInfo(result, cur, FLAT_PROFILE_OWN, infos, mapping);

			//childs
			std::map<std::string, bool> done;
			for (size_t j = 0 ; j < stack.size() ; j++)
			{
				//skip firsts for 'own' mode, otherwise keep them
				if (j <= skip)
					continue;

				//extract some quick refs
				const LangAddress cur = stack[j];
				const std::string key = mapping(addrTranslation.at(cur), infos);
				bool accepted = filter(addrTranslation.at(cur), infos);
				if (accepted && done.find(key) == done.end())
				{
					done[key] = true;
					this->mergeStackInfo(result,cur,FLAT_PROFILE_TOTAL,infos,mapping);
				}
			}
		}

		//merge
		#pragma omp critical
		{
			for (auto & it : result)
				resultGeneral[it.first].merge(it.second);
		}
	}

	//convert to simple list
	FlatProfileVector resultVector;
	resultVector.reserve(resultGeneral.size());
	for (const auto & it : resultGeneral)
		resultVector.emplace_back(it.second);

	//ok return
	return resultVector;
}

/**********************************************************/
void FlatProfileValue::merge(const FlatProfileValue & value)
{
	//merge
	this->own.merge(value.own);
	this->total.merge(value.total);
	//keep lowest line
	if (this->location == nullptr)
		this->location = value.location;
	else if (value.location != nullptr && value.location->line != -1 && value.location->line < this->location->line)
		this->location = value.location;
}

/**********************************************************/
void Extractor::mergeStackInfo(FlatProfileMap & into, const LangAddress & addr,FlatProfileCounter counter,const StackInfos & infos,const LocaltionMappingFunc & mapping) const
{
	//extract key by using mapping function
	const InstructionInfosStrRef & detailedStackEntry = this->addrTranslation.at(addr);
	std::string key = mapping(detailedStackEntry, infos);
	if (key.empty())
		key = MALTFormat::to_string(addr);

	//check existing
	auto it = into.find(key);
	FlatProfileValue * cur{nullptr};
	if (it == into.end())
	{
		//build it
		cur = &into[key];

		//copy user requested fields
		cur->location = &detailedStackEntry;
	} else {
		cur = &it->second;
		//check line and keep the lowest one
		if (detailedStackEntry.line != 0 && detailedStackEntry.line != -1 && (detailedStackEntry.line < cur->location->line || cur->location->line == -1 || cur->location->line == 0))
			cur->location = &detailedStackEntry;
	}

	//check for subkey (own or total) and clone or merge
	switch(counter) {
		case FLAT_PROFILE_TOTAL:
			cur->total.merge(infos);
			break;
		case FLAT_PROFILE_OWN:
			cur->own.merge(infos);
			break;
		default:
			break;
	}
}

/**********************************************************/
void to_json(nlohmann::json & json, const InstructionInfosStrRef & value)
{
	json = nlohmann::json{
		{"binary", *value.binary},
		{"file", *value.file},
		{"function", *value.function},
		{"line", value.line},
	};
}

/**********************************************************/
void to_json(nlohmann::json & json, const FlatProfileValue & value)
{
	json = nlohmann::json{
		{"own", value.own},
		{"total", value.total},
		{"location", *value.location},
	};
}

/**********************************************************/
void to_json(nlohmann::json & json, const InstructionInfosStrRef* value)
{
	to_json(json, *value);
}

/**********************************************************/
void to_json(nlohmann::json & json, const ProcMapDistrEntry & value)
{
	json = nlohmann::json{
		{"mem", value.mem},
		{"cnt", value.cnt},
	};
}

/**********************************************************/
void to_json(nlohmann::json & json, const FilteredStackEntry & value)
{
	json = nlohmann::json{
		{"mem", value.stack},
		{"cnt", value.infos},
	};
}

/**********************************************************/
void to_json(nlohmann::json & json, const TimedValues & value)
{
	json = nlohmann::json{
		{"ticksPerSecond", value.ticksPerSecond},
		{"freeBandwidth", value.freeBandwidth},
		{"memoryBandwidth", value.memoryBandwidth},
		{"memoryTimeline", value.memoryTimeline},
		{"systemTimeline", value.systemTimeline},
	};
}

/**********************************************************/
void to_json(nlohmann::json & json, const SummaryV2 & value)
{
	json["run"] = value.run;
	json["system"] = nlohmann::json{
		{"totalMemory", value.system.totalMemory},
		{"ticksPerSecond", value.system.ticksPerSecond},
	};
	json["summary"] = nlohmann::json{
		{"peakPhysicalMemory", value.summary.peakPhysicalMemory},
		{"peakVirtualMemory", value.summary.peakVirtualMemory},
		{"peakRequestedMemory", value.summary.peakRequestedMemory},
		{"peakSegmentCount", value.summary.peakSegmentCount},
		{"peakAllocRate", value.summary.peakAllocRate},
		{"peakAllocCountRate", value.summary.peakAllocCountRate},
		{"totalAllocatedMemory", value.summary.totalAllocatedMemory},
		{"recyclingRatio", value.summary.recyclingRatio},
		{"allocCount", value.summary.allocCount},
		{"minAllocSize", value.summary.minAllocSize},
		{"meanAllocSize", value.summary.meanAllocSize},
		{"maxAllocSize", value.summary.maxAllocSize},
		{"leakedMem", value.summary.leakedMem},
		{"leakedCount", value.summary.leakedCount},
		{"largestStack", value.summary.largestStack},
		{"numGblVar", value.summary.numGblVar},
		{"globalVarMem", value.summary.globalVarMem},
		{"tlsVarMem", value.summary.tlsVarMem},
	};
	json["summaryWarnings"] = value.summaryWarnings;
	json["threadStats"] = value.threadStats;
}

/**********************************************************/
void to_json(nlohmann::json & json, const Summary & value)
{
	json["globalStats"] = nlohmann::json{
		{"physicalMem", value.globalStats.physicalMem},
		{"virtualMem", value.globalStats.virtualMem},
		{"requestedMem", value.globalStats.requestedMem},
		{"internalMemory", value.globalStats.internalMemory},
		{"segments", value.globalStats.segments},
		{"minChunkSize", value.globalStats.minChunkSize},
		{"maxChunkSize", value.globalStats.maxChunkSize},
		{"count", value.globalStats.count},
		{"largestStack", value.globalStats.largestStack},
		{"cumulAllocs", value.globalStats.cumulAllocs},
	};
}

/**********************************************************/
void to_json(nlohmann::json & json, const FlattenMaxStackInfo & value)
{
	json = nlohmann::json{
		{"details", value.details},
		{"totalMem", value.totalMem}
	};
}

/**********************************************************/
void to_json(nlohmann::json & json, const FlattenMaxStackInfoEntry & value)
{
	json = nlohmann::json{
		{"info", *value.info},
		{"mem", value.mem},
		{"count", value.count},
	};
}

/**********************************************************/
const std::string& Extractor::getString(ssize_t id) const
{
	//check
	assert(id >= -1);
	assert(id < (ssize_t)this->profile.sites.strings.size());

	//apply
	if (id == -1)
		return unknown;
	else
		return this->profile.sites.strings[id];
}

/**********************************************************/
FunctionStackVector Extractor::getDebugStackList() const
{
	//vars
	FunctionStackVector res;
	const auto & stats = this->profile.stacks.stats;

	//pre allocate
	res.reserve(stats.size());
	for (size_t i = 0 ; i < stats.size() ; i++) {
		res.emplace_back();
	}

	//loop on all
	#pragma omp parallel for
	for (size_t i = 0 ; i < stats.size() ; i++) {
		//vars
		const auto & statEntry = stats[i];
		auto & outStack = res[i];

		//pre allocate
		outStack.reserve(statEntry.stack.size());

		//fill
		for (const auto & addr : statEntry.stack) {
			outStack.push_back(*this->addrTranslation.at(addr).function);
		}
	}

	//ok
	return res;
}

/**********************************************************/
ProcMapDistr Extractor::getProcMapDistr(void) const
{
	//some local vars
	ProcMapDistr res;
	const auto & map = this->profile.sites.map;
	std::regex isStackRegexp("^\\[stack:[0-9]+\\]$");

	//loop on map entries
	for (const auto & it : map)
	{
		//compte mem
		size_t mem = (char*)it.upper - (char*)it.lower;

		//check goal
		std::string file = it.file;
		if (file.empty())
			file = "anonymous";
		if (std::regex_search(file, isStackRegexp))
			file = "stack";

		//sum
		res[file].mem += mem;
		res[file].cnt++;
	}

	//ok return
	return res;
}

/**********************************************************/
bool Extractor::filterExtractStacksCandidate(const Stack & stack, const LocaltionOnlyFilterFunc & filter) const
{
	for (const auto & it : stack)
	{
		const InstructionInfosStrRef & ref = this->addrTranslation.at(it);
		if (filter(ref))
			return true;
	}
	return false;
}

/**********************************************************/
StackStrRef Extractor::buildStackStrRef(const Stack & stack) const
{
	//build
	StackStrRef res;
	res.reserve(stack.size());

	//convert
	for (const auto & it : stack)
		res.push_back(&this->addrTranslation.at(it));

	//ok
	return res;
}

/**********************************************************/
FilteredStackList Extractor::getFilterdStacks(const LocaltionOnlyFilterFunc & filter) const
{
	//get some refs
	const auto & stats = this->profile.stacks.stats;
	FilteredStackList res;	

	//loop on stat entries
	#pragma omp parallel for
	for (const auto & statEntry : stats)
	{
		//get stack
		const Stack & stack = statEntry.stack;
		const StackInfos & infos = statEntry.infos;

		//check if conteain
		if (filterExtractStacksCandidate(stack,filter)) {
			FilteredStackEntry entry{this->buildStackStrRef(stack), infos};
			#pragma omp critical
			res.emplace_back(std::move(entry));
		}
	}

	return res;
}

/**********************************************************/
FilteredStackList Extractor::getFilterdStacksOnFileLine(const std::string & file, size_t line) const
{
	return this->getFilterdStacks([&file, line](const InstructionInfosStrRef & location) {
		return *location.file == file && location.line == line;
	});
}

/**********************************************************/
TimedValues Extractor::getTimedValues(void) const
{
	TimedValues tmp;
	tmp.ticksPerSecond = this->profile.globals.ticksPerSecond;
	tmp.memoryBandwidth = this->profile.timeline.memoryBandwidth;
	tmp.memoryTimeline = this->profile.timeline.memoryTimeline;
	tmp.systemTimeline = this->profile.timeline.systemTimeline;
	return tmp;
}

/**********************************************************/
/**
 * Get info about the largest stack
**/
const MALTFormat::ThreadStackMem & Extractor::getMaxStack(void) const
{
	//get first to start
	size_t id = 0;

	//loop
	for (size_t i = 0 ; i < this->profile.threads.size() ; i++)
	{
		const auto & tmp = this->profile.threads[i];
		if (tmp.stackMem.size > this->profile.threads[id].stackMem.size)
			id = i;
	}

	return this->profile.threads[id].stackMem;
}

/**********************************************************/
SummaryV2 Extractor::getSummaryV2(void) const
{
	SummaryV2 ret;

	//extract run info
	ret.run = this->profile.run;

	//extract system info
	ret.system.totalMemory = this->profile.globals.totalMemory;
	ret.system.ticksPerSecond = this->profile.globals.ticksPerSecond;

	//summary
	ret.summary = {};
	ret.summary.peakPhysicalMemory = this->profile.timeline.memoryTimeline.peak[2];
	ret.summary.peakVirtualMemory = this->profile.timeline.memoryTimeline.peak[1];
	ret.summary.peakRequestedMemory = this->profile.timeline.memoryTimeline.peak[0];
	ret.summary.peakInternalMemory = this->profile.timeline.memoryTimeline.peak[3];
	ret.summary.peakSegmentCount = this->profile.timeline.memoryTimeline.peak[4];

	//rates
	size_t peakMem = 0;
	for (const auto & value : this->profile.timeline.memoryBandwidth.values)
		peakMem = std::max(peakMem, value[1]);
	ret.summary.peakAllocRate = ((float)peakMem / (float)this->profile.timeline.memoryBandwidth.perPoints) * this->profile.globals.ticksPerSecond;

	//rates
	size_t peakCount = 0;
	for (const auto & value : this->profile.timeline.memoryBandwidth.values)
		peakCount = std::max(peakCount, value[3]);
	ret.summary.peakAllocCountRate = ((float)peakCount / (float)this->profile.timeline.memoryBandwidth.perPoints) * this->profile.globals.ticksPerSecond;

	//search min/max/count size
	ssize_t min = -1;
	ssize_t max = -1;
	ssize_t count = 0;
	const MALTFormat::StackStats & stats = this->profile.stacks.stats;
	size_t sum = 0;
	for(const auto & it : stats)
	{
		const StackInfos & info = it.infos;
		if ((info.alloc.min < min || min == -1) && info.alloc.min > 0)
			min = info.alloc.min;
		if (info.alloc.max > max || max == -1)
			max = info.alloc.max;
		count += info.alloc.count;
		sum += info.alloc.sum;
	}

	//gen
	ret.summary.totalAllocatedMemory = sum;
	ret.summary.recyclingRatio = sum / ret.summary.peakRequestedMemory;
	ret.summary.allocCount = count;
	ret.summary.minAllocSize = min;
	ret.summary.meanAllocSize = sum / count;
	ret.summary.maxAllocSize = max;

	//leaks
	size_t leakCount = 0;
	size_t leakMem = 0;
	const Leaks & leaks = this->profile.leaks;
	for (const auto & it : leaks)
	{
		leakCount += it.count;
		leakMem += it.memory;
	}
	ret.summary.leakedMem = leakMem;
	ret.summary.leakedCount = leakCount;

	//stacks
	ret.summary.largestStack = this->getMaxStack().size;

	//global vars
	size_t tlsMem = 0;
	size_t gblMem = 0;
	size_t cntVars = 0;
	const auto & gvars = this->profile.memStats.globalVariables;
	for (const auto & it : gvars)
	{
		for (const auto & var : it.second)
		{
			cntVars++;
			if (var.tls)
				tlsMem += var.size;
			else
				gblMem += var.size;
		}
	}
	ret.summary.numGblVar = cntVars;
	ret.summary.globalVarMem = gblMem;
	ret.summary.tlsVarMem = tlsMem * (this->profile.globals.maxThreadCount + 1);

	//summary warnings
	ret.summaryWarnings = this->genSummaryWarnings(ret);

	//thread stats
	for (const auto & it : this->profile.threads)
		ret.threadStats.emplace_back(it.stats);

	//return
	return ret;
}

/**********************************************************/
SummaryWarnings Extractor::genSummaryWarnings(const SummaryV2 & data) const
{
	//vars
	SummaryWarnings ret;

	//calc
	float runtime = (float)data.run.runtime / (float)data.system.ticksPerSecond;

	//check too large recycling ratio
	if (data.summary.recyclingRatio > 10)
	{
		ret["recyclingRatio"].push_back("Caution, you are heavily recycling your memory, it might hurt performance, check the allocation rate.");
		ret["totalAllocatedMemory"].push_back("Caution, you are heavily recycling your memory, it might hurt performance, check the allocation rate.");
	}
	if (data.summary.allocCount / runtime > 100000)
		ret["allocCount"].push_back("Caution, you are doing really large number of memory allocation, it might hurt performance.");
	if (data.summary.leakedMem > data.summary.peakRequestedMemory / 2)
		ret["leakMem"].push_back("Caution, half of your memory has leaked, it might not be an issue, but maybe you need to ensure the segments are used during the whole program life.");
	if (data.summary.globalVarMem > data.summary.peakRequestedMemory / 3 && data.summary.globalVarMem > 1024*1024)
		ret["globalVarMem"].push_back("Caution, a large part of your memory is consummed by global variables, check if it is normal.");
	if (data.summary.tlsVarMem > data.summary.peakRequestedMemory / 3 && data.summary.tlsVarMem > 1024*1024)
		ret["tlsVarMem"].push_back("Caution, a large part of your memory is consummed by TLS variables, check if it is normal.");
	if (data.summary.numGblVar > 500)
		ret["numGblVar"].push_back("Caution, you get a realy big number of global variable, your code is likely to be buggy.");

	return ret;
}

/**********************************************************/
Summary Extractor::getSummary(void) const
{
	Summary ret;

	//extract global stats
	ret.globalStats = {};
	ret.globalStats.physicalMem = this->profile.timeline.memoryTimeline.peak[2];//this->profile.timeline.physicalMem.peakMemory;
	ret.globalStats.virtualMem = this->profile.timeline.memoryTimeline.peak[1];//this->profile.timeline.virtualMem.peakMemory;
	ret.globalStats.requestedMem = this->profile.timeline.memoryTimeline.peak[0];//this->profile.timeline.requestedMem.peakMemory;
	ret.globalStats.internalMemory = this->profile.timeline.memoryTimeline.peak[4];//this->profile.timeline.internalMem.peakMemory;
	ret.globalStats.segments = this->profile.timeline.memoryTimeline.peak[3];//this->profile.timeline.segments.peakMemory;

	//search min/max/count size
	ssize_t min = -1;
	ssize_t max = -1;
	size_t count = 0;
	const auto & stats = this->profile.stacks.stats;
	size_t sum = 0;
	for (const auto & it : stats)
	{
		const auto & info = it.infos;
		if ((info.alloc.min < min || min == -1) && info.alloc.min > 0)
			min = info.alloc.min;
		if (info.alloc.max > max || max == -1)
			max = info.alloc.max;
		count += info.alloc.count;
		sum += info.alloc.sum;
	}

	//extract
	ret.globalStats.minChunkSize = min;
	ret.globalStats.maxChunkSize = max;
	ret.globalStats.count = count;
	ret.globalStats.largestStack = this->getMaxStack().size;
	ret.globalStats.cumulAllocs = sum;

	return ret;
}

/**********************************************************/
FlattenMaxStackInfo Extractor::getFlattenMaxStackInfo(const LocaltionOnlyMappingFunc & mapping,const LocaltionOnlyFilterFunc & accept, const ThreadStackMem & maxStack)
{
	//init hash map to flat on addresses
	std::map<std::string, FlattenMaxStackInfoEntry> ret;
	//var maxStack = this.data.maxStack;
	//var maxStack = this.getMaxStack();

	//loop on all entries
	for (size_t i = 0 ; i < maxStack.stack.size() ; i++)
	{
		//get some vars
		LangAddress addr = maxStack.stack[i];
		ssize_t mem = maxStack.mem[i] - maxStack.mem[i+1];
		assert(mem >= 0);
		const InstructionInfosStrRef & info = this->addrTranslation[addr];
		std::string key = to_string(addr);
		//if (info != undefined)
		key = mapping(info);
		//else
		//info = {function:addr};

		//check filter
		if (accept(info)) {
			auto it = ret.find(key);
			//create or merge
			if (it == ret.end()) {
				ret[key] = FlattenMaxStackInfoEntry{&info, 1, (size_t)mem};
			} else {
				ret[key].mem += mem;
				ret[key].count++;
			}
		}
	}

	//remove keys
	std::vector<FlattenMaxStackInfoEntry> finalRes;
	for (const auto & it : ret)
		finalRes.push_back(it.second);

	//ok return
	return FlattenMaxStackInfo{finalRes,maxStack.size};
}

/**********************************************************/
FlattenMaxStackInfo Extractor::getMaxStackInfoOnFunction(void)
{
	return this->getFlattenMaxStackInfo(
		[](const InstructionInfosStrRef & location) {return *location.function;},
		[](const InstructionInfosStrRef & location) {return true;},
		this->getMaxStack()
	);
}

/**********************************************************/
/**
 * Flatten datas about the largest stack and return as json tree.
**/
FlattenMaxStackInfo Extractor::getStackInfoOnFunction(size_t id)
{
	return this->getFlattenMaxStackInfo(
		[](const InstructionInfosStrRef & location) {return *location.function;},
		[](const InstructionInfosStrRef & location) {return true;},
		this->profile.threads[id].stackMem
	);
}

/**********************************************************/
SourceFileMap Extractor::getSourceFileMap(void)
{
	//vars
	SourceFileMap sourceFiles;

	//loop
	for (const auto & site : this->profile.sites.instr)
	{
		//skip
		if (site.second.file == -1)
			continue;

		//get as string
		std::string source = this->getString(site.second.file);
		sourceFiles[source] = true;
	}

	//ok
	return sourceFiles;
}

}
