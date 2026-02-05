/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 01/2026
*    LICENSE  : CeCILL-C
*    FILE     : src/reader/libreader/extractors/Extractor.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
*    AUTHOR   : Sébastien Valat - 2026
***********************************************************/

#ifndef MALT_READER_EXTRACTOR_HPP
#define MALT_READER_EXTRACTOR_HPP

/**********************************************************/
#include <functional>
#include <vector>
#include <cstdlib>
#include <nlohmann/json.hpp>
#include "../format/MaltProfile.hpp"

/**********************************************************/
namespace MALTReader
{

/**********************************************************/
class CallTreeAdapter;

/**********************************************************/
enum FlatProfileCounter
{
	FLAT_PROFILE_OWN,
	FLAT_PROFILE_TOTAL,
	FLAT_PROFILE_CHILDS,
};

/**********************************************************/
struct InstructionInfosStrRef
{
	const std::string * file{nullptr};
	const std::string * binary{nullptr};
	const std::string * function{nullptr};
	const std::string * functionShort{nullptr};
	ssize_t line{-1};
	MALTFormat::LangAddress origin{MALTFormat::LANG_C, nullptr};
	size_t offset{0};
};

/**********************************************************/
struct FlatProfileValue
{
	MALTFormat::StackInfos childs;
	MALTFormat::StackInfos own;
	MALTFormat::StackInfos total;
	const InstructionInfosStrRef * location{nullptr}; 
	void merge(const FlatProfileValue & value);
};

/**********************************************************/
struct ProcMapDistrEntry
{
	size_t mem{0};
	size_t cnt{0};
};

/**********************************************************/
struct TimedValues
{
	MALTFormat::CyclesDuration ticksPerSecond;
	size_t allocBandwidth;
	size_t freeBandwidth;
	MALTFormat::TimelineInfos memoryBandwidth;
	MALTFormat::TimelineInfos memoryTimeline;
	MALTFormat::TimelineInfos systemTimeline;
};

/**********************************************************/
typedef std::map<std::string, std::list<std::string> > SummaryWarnings;

/**********************************************************/
struct SummaryV2
{
	struct {
		std::string filename;
	} profile;
	MALTFormat::Run run;
	struct {
		size_t totalMemory{0};
		MALTFormat::CyclesDuration ticksPerSecond{0};
	} system;
	struct {
		size_t peakPhysicalMemory{0};
		size_t peakVirtualMemory{0};
		size_t peakRequestedMemory{0};
		size_t peakInternalMemory{0};
		size_t peakSegmentCount{0};
		float peakAllocRate{0};
		float peakAllocCountRate{0};
		size_t totalAllocatedMemory{0};
		float recyclingRatio{0};
		size_t allocCount{0};
		size_t minAllocSize{0};
		float meanAllocSize{0};
		size_t maxAllocSize{0};
		size_t leakedMem{0};
		size_t leakedCount{0};
		size_t largestStack{0};
		size_t numGblVar{0};
		size_t globalVarMem{0};
		size_t tlsVarMem{0};
		size_t maxThreadCount{0};
	} summary;
	SummaryWarnings summaryWarnings;
	std::vector<MALTFormat::ThreadsStats> threadStats;
	MALTFormat::Domains summaryDomains;
};

/**********************************************************/
struct Summary
{
	struct{
		size_t physicalMem{0};
		size_t virtualMem{0};
		size_t requestedMem{0};
		size_t internalMemory{0};
		size_t segments{0};
		size_t minChunkSize{0};
		size_t maxChunkSize{0};
		size_t count{0};
		size_t largestStack{0};
		size_t cumulAllocs{0};
	} globalStats;
};

/**********************************************************/
typedef std::vector<const InstructionInfosStrRef*> StackStrRef;

/**********************************************************/
struct FilteredStackEntry
{
	StackStrRef stack;
	MALTFormat::StackInfos infos;
};

/**********************************************************/
struct FlattenMaxStackInfoEntry
{
	const InstructionInfosStrRef * info;
	size_t mem;
	size_t count;
};

/**********************************************************/
struct FlattenMaxStackInfo
{
	std::vector<FlattenMaxStackInfoEntry> details;
	size_t totalMem;
};

/**********************************************************/
struct FullTreeNode
{
	std::map<MALTFormat::LangAddress, FullTreeNode> child;
	std::unique_ptr<MALTFormat::StackInfos> infos{nullptr};
	const InstructionInfosStrRef * location{nullptr};
};

/**********************************************************/
struct CallStackChild
{
	CallStackChild(MALTFormat::StackInfos infos, const InstructionInfosStrRef * location, size_t parentStackId, size_t parentStackDepth, size_t stackId, size_t stackDepth, bool hasChild);
	MALTFormat::StackInfos infos;
	const InstructionInfosStrRef * location{nullptr};
	size_t parentStackId{0};
	size_t parentStackDepth{0};
	size_t stackId{0};
	size_t stackDepth{0};
	bool hasChild;
};

/**********************************************************/
struct StackMem
{
	std::vector<MALTFormat::ThreadStackMem> stacks;
	MALTFormat::CyclesDuration ticksPerSecond;
};

/**********************************************************/
typedef std::map<std::string, ProcMapDistrEntry> ProcMapDistr;
typedef std::list<FilteredStackEntry> FilteredStackList;
typedef std::list<CallStackChild> CallStackChildList;

/**********************************************************/
typedef std::function<std::string(const InstructionInfosStrRef & /*location*/, const MALTFormat::StackInfos & /*infos*/)> LocaltionMappingFunc;
typedef std::function<std::string(const InstructionInfosStrRef & /*location*/)> LocaltionOnlyMappingFunc;
typedef std::function<bool(const InstructionInfosStrRef & /*location*/, const MALTFormat::StackInfos & /*infos*/)> LocaltionFilterFunc;
typedef std::function<bool(const InstructionInfosStrRef & /*location*/)> LocaltionOnlyFilterFunc;
typedef std::map<std::string, FlatProfileValue> FlatProfileMap;
typedef std::vector<FlatProfileValue> FlatProfileVector;
typedef std::vector<std::vector<std::string> > FunctionStackVector;
typedef std::map<std::string, bool> SourceFileMap;

/**********************************************************/
struct LocationFilter
{
	std::string function;
	std::string file;
	ssize_t line{-1};
};

/**********************************************************/
struct Link
{
	MALTFormat::LangAddress in;
	MALTFormat::LangAddress out;
	bool hasSkipedNodes;
};

/**********************************************************/
inline bool operator<(const Link & a, const Link & b)
{
	if (a.in == b.in)
		return a.out < b.out;
	else
		return a.in < b.in;
}

/**********************************************************/
inline bool operator==(const Link & a, const Link & b)
{
	return (a.in == b.in) && (a.out == b.out);
}

/**********************************************************/
struct NodeRef
{
	NodeRef(ssize_t nodeId)
	{
		if (nodeId == -1) {
			this->stackId = 0;
			this->offset = -1;
		} else {
			//due to 0 beeing reserved for another use to give function name as ref
			nodeId -= 1;
			this->stackId = nodeId / (1UL<<32);
			this->offset = nodeId % (1UL<<32);
		}
	}

	ssize_t getId(void) const
	{
		if (offset == -1)
			return -1;
		else
			return this->stackId * (1UL<<32) + this->offset % (1UL<<32) + 1;
	}
	
	size_t stackId;
	ssize_t offset;
};

class Graph;

/**********************************************************/
class Extractor
{
	public:
		Extractor(MALTFormat::MaltProfile & profile, const std::string & filename);
		~Extractor(void);
		FlatProfileVector getFlatProfile(const LocaltionMappingFunc & mapping,const LocaltionFilterFunc & filter) const;
		ProcMapDistr getProcMapDistr(void) const;
		FunctionStackVector getDebugStackList() const;
		FilteredStackList getFilterdStacks(const LocaltionOnlyFilterFunc & filter) const;
		FilteredStackList getFilterdStacksOnFileLine(const std::string & file, size_t line) const;
		FilteredStackList getFilterdStacksOnSymbol(const std::string & func) const;
		TimedValues getTimedValues(void) const;
		SummaryWarnings genSummaryWarnings(const SummaryV2 & data) const;
		SummaryV2 getSummaryV2(void) const;
		Summary getSummary(void) const;
		FlattenMaxStackInfo getFlattenMaxStackInfo(const LocaltionOnlyMappingFunc & mapping,const LocaltionOnlyFilterFunc & accept, const MALTFormat::ThreadStackMem & maxStack);
		FlattenMaxStackInfo getMaxStackInfoOnFunction(void);
		FlattenMaxStackInfo getStackInfoOnFunction(size_t id);
		SourceFileMap getSourceFileMap(void) const;
		FullTreeNode getFullTree(void) const;
		nlohmann::json getCallTree(ssize_t nodeId, ssize_t depth, ssize_t height, double minCost, const std::string & func, const std::string & metric, bool isRatio);
		CallStackChildList getCallStackNextLevel(size_t parentStackId, size_t parentDepth, const LocationFilter & filter) const;
		const MALTFormat::ThreadStackMem & getMaxStack(void) const;
		StackMem getStackMem(void) const;
		const MALTFormat::MaltProfile & getProfile(void) const;
		size_t toVirtualAddress(const std::string & binaryObject, size_t inObjectaddress);
		bool toVirtualAddresses(std::vector<size_t> & addresses, const std::string & binaryObject);
	private:
		Graph getFilteredTree(ssize_t nodeId, ssize_t depth, ssize_t height, double minCost, const std::string & metric, bool isRatio) const;
		inline const InstructionInfosStrRef & getAddrTranslation(MALTFormat::LangAddress addr) const;
		void mergeStackInfo(FlatProfileMap & into, const MALTFormat::LangAddress & addr,FlatProfileCounter counter,const MALTFormat::StackInfos & infos,const LocaltionMappingFunc & mapping) const;
		void buildTranslation(MALTFormat::MaltProfile & profile);
		void buildTranslation(MALTFormat::Stack & stack);
		const std::string& getString(ssize_t id) const;
		bool filterExtractStacksCandidate(const MALTFormat::Stack & stack, const LocaltionOnlyFilterFunc & filter) const;
		StackStrRef buildStackStrRef(const MALTFormat::Stack & stack) const;
		const std::string& getCachedString(const std::string & value);
		bool stackIsMatchingBellowDepth(const MALTFormat::Stack & stack1, const MALTFormat::Stack & stack2, size_t depth) const;
		bool stackIsMatchingLocationFilter(const LocationFilter & filter, const MALTFormat::Stack & stack) const;
		size_t getBinaryOffset(const MALTFormat::LangAddress & langAddress);
	private:
		const MALTFormat::MaltProfile & profile;
		std::map<MALTFormat::LangAddress, InstructionInfosStrRef> addrTranslationHidden;
		std::string unknown{"??"};
		CallTreeAdapter * calltreeCache{nullptr};
		std::map<std::string, bool> cachedStrings;
		std::string filename;
};

/**********************************************************/
void to_json(nlohmann::json & json, const InstructionInfosStrRef & value);
void to_json(nlohmann::json & json, const FlatProfileValue & value);
void to_json(nlohmann::json & json, const ProcMapDistrEntry & value);
void to_json(nlohmann::json & json, const FilteredStackEntry & value);
void to_json(nlohmann::json & json, const TimedValues & value);
void to_json(nlohmann::json & json, const SummaryV2 & value);
void to_json(nlohmann::json & json, const Summary & value);
void to_json(nlohmann::json & json, const FlattenMaxStackInfo & value);
void to_json(nlohmann::json & json, const FlattenMaxStackInfoEntry & value);
void to_json(nlohmann::json & json, const FullTreeNode & value);
void to_json(nlohmann::json & json, const CallStackChild & value);
void to_json(nlohmann::json & json, const StackMem & value);

/**********************************************************/
const InstructionInfosStrRef & Extractor::getAddrTranslation(MALTFormat::LangAddress addr) const
{
	assert(addr.lang == MALTFormat::LANG_TRANS_PTR);
	const InstructionInfosStrRef * infos = (InstructionInfosStrRef*)addr.address;
	assert(this->addrTranslationHidden.find(infos->origin) != this->addrTranslationHidden.end());
	//if (addr.lang == MALTFormat::LANG_TRANS_PTR)
	return *infos;
	/*else
		return this->addrTranslationHidden.at(addr);*/
}

}

#endif //MALT_READER_EXTRACTOR_HPP
