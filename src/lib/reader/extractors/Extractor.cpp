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

}
