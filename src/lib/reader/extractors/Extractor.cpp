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

			//check
			assert(instrInfos.binary != -1);
			assert(instrInfos.file != -1);
			assert(instrInfos.function != -1);
			assert(instrInfos.binary < strings.size());
			assert(instrInfos.file < strings.size());
			assert(instrInfos.function < strings.size());

			//build ref
			InstructionInfosStrRef ref;
			ref.binary = &strings[instrInfos.binary];
			ref.function = &strings[instrInfos.function];
			ref.file = &strings[instrInfos.file];
			ref.line = instrInfos.line;

			//store
			this->addrTranslation[addr] = ref;
		}
	}
}

/**********************************************************/
FlatProfileVector Extractor::getFlatProfile(const LocaltionMappingFunc & mapping,const LocaltionFilterFunc & filter) const
{
	//vars
	FlatProfileMap result;
	const StackStats & stats = this->profile.stacks.stats;

	//loop on all
	for (const auto & statEntry : stats) {
		//extract some short refs
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

	//convert to simple list
	FlatProfileVector resultVector;
	resultVector.reserve(result.size());
	for (const auto & it : result)
		resultVector.emplace_back(it.second);

	//ok return
	return resultVector;
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

}
