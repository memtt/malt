/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 10/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/tools/Backtrace.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

#ifndef MALT_READER_EXTRACTOR_HPP
#define MALT_READER_EXTRACTOR_HPP

/**********************************************************/
#include <functional>
#include <vector>
#include <nlohmann/json.hpp>
#include "../format/MaltProfile.hpp"

/**********************************************************/
namespace MALTReader
{

/**********************************************************/
enum FlatProfileCounter
{
	FLAT_PROFILE_OWN,
	FLAT_PROFILE_TOTAL,
};

/**********************************************************/
struct InstructionInfosStrRef
{
	const std::string * file{nullptr};
	const std::string * binary{nullptr};
	const std::string * function{nullptr};
	ssize_t line{-1};
	MALTFormat::LangAddress origin{MALTFormat::LANG_C, nullptr};
};

/**********************************************************/
struct FlatProfileValue
{
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
	MALTFormat::Run run;
	struct {
		size_t totalMemory;
		MALTFormat::CyclesDuration ticksPerSecond;
	} system;
	struct {
		size_t peakPhysicalMemory;
		size_t peakVirtualMemory;
		size_t peakRequestedMemory;
		size_t peakInternalMemory;
		size_t peakSegmentCount;
		float peakAllocRate;
		size_t peakAllocCountRate;
		size_t totalAllocatedMemory;
		float recyclingRatio;
		size_t allocCount;
		size_t minAllocSize;
		size_t meanAllocSize;
		size_t maxAllocSize;
		size_t leakedMem;
		size_t leakedCount;
		size_t largestStack;
		size_t numGblVar;
		size_t globalVarMem;
		size_t tlsVarMem;
	} summary;
	SummaryWarnings summaryWarnings;
	std::vector<MALTFormat::ThreadsStats> threadStats;
};

/**********************************************************/
struct Summary
{
	struct{
		size_t physicalMem;
		size_t virtualMem;
		size_t requestedMem;
		size_t internalMemory;
		size_t segments;
		size_t minChunkSize;
		size_t maxChunkSize;
		size_t count;
		size_t largestStack;
		size_t cumulAllocs;
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
typedef std::map<std::string, ProcMapDistrEntry> ProcMapDistr;
typedef std::list<FilteredStackEntry> FilteredStackList;

/**********************************************************/
typedef std::function<std::string(const InstructionInfosStrRef & /*location*/, const MALTFormat::StackInfos & /*infos*/)> LocaltionMappingFunc;
typedef std::function<std::string(const InstructionInfosStrRef & /*location*/)> LocaltionOnlyMappingFunc;
typedef std::function<bool(const InstructionInfosStrRef & /*location*/, const MALTFormat::StackInfos & /*infos*/)> LocaltionFilterFunc;
typedef std::function<bool(const InstructionInfosStrRef & /*location*/)> LocaltionOnlyFilterFunc;
typedef std::map<std::string, FlatProfileValue> FlatProfileMap;
typedef std::vector<FlatProfileValue> FlatProfileVector;
typedef std::vector<std::vector<std::string> > FunctionStackVector;

/**********************************************************/
class Extractor
{
	public:
		Extractor(const MALTFormat::MaltProfile & profile);
		~Extractor(void);
		FlatProfileVector getFlatProfile(const LocaltionMappingFunc & mapping,const LocaltionFilterFunc & filter) const;
		ProcMapDistr getProcMapDistr(void) const;
		FunctionStackVector getDebugStackList() const;
		FilteredStackList getFilterdStacks(const LocaltionOnlyFilterFunc & filter) const;
		FilteredStackList getFilterdStacksOnFileLine(const std::string & file, size_t line) const;
		TimedValues getTimedValues(void) const;
		SummaryWarnings genSummaryWarnings(const SummaryV2 & data) const;
		SummaryV2 getSummaryV2(void) const;
		Summary getSummary(void) const;
		FlattenMaxStackInfo getFlattenMaxStackInfo(const LocaltionOnlyMappingFunc & mapping,const LocaltionOnlyFilterFunc & accept, const MALTFormat::ThreadStackMem & maxStack);
		FlattenMaxStackInfo getMaxStackInfoOnFunction(void);
		FlattenMaxStackInfo getStackInfoOnFunction(size_t id);
	private:
		void mergeStackInfo(FlatProfileMap & into, const MALTFormat::LangAddress & addr,FlatProfileCounter counter,const MALTFormat::StackInfos & infos,const LocaltionMappingFunc & mapping) const;
		void buildTranslation(void);
		const std::string& getString(ssize_t id) const;
		bool filterExtractStacksCandidate(const MALTFormat::Stack & stack, const LocaltionOnlyFilterFunc & filter) const;
		StackStrRef buildStackStrRef(const MALTFormat::Stack & stack) const;
		const MALTFormat::ThreadStackMem & getMaxStack(void) const;
	private:
		const MALTFormat::MaltProfile & profile;
		std::map<MALTFormat::LangAddress, InstructionInfosStrRef> addrTranslation;
		std::string unknown{"??"};
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

}

#endif //MALT_READER_EXTRACTOR_HPP
