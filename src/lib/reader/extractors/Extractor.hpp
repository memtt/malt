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
};

/**********************************************************/
struct FlatProfileValue
{
	MALTFormat::StackInfos own;
	MALTFormat::StackInfos total;
	const InstructionInfosStrRef * location{nullptr}; 
};

/**********************************************************/
typedef std::function<std::string(const InstructionInfosStrRef & /*location*/, const MALTFormat::StackInfos & /*infos*/)> LocaltionMappingFunc;
typedef std::function<bool(const InstructionInfosStrRef & /*location*/, const MALTFormat::StackInfos & /*infos*/)> LocaltionFilterFunc;
typedef std::map<std::string, FlatProfileValue> FlatProfileMap;
typedef std::vector<FlatProfileValue> FlatProfileVector;

/**********************************************************/
class Extractor
{
	public:
		Extractor(const MALTFormat::MaltProfile & profile);
		~Extractor(void);
		FlatProfileVector getFlatProfile(const LocaltionMappingFunc & mapping,const LocaltionFilterFunc & filter) const;
	private:
		void mergeStackInfo(FlatProfileMap & into, const MALTFormat::LangAddress & addr,FlatProfileCounter counter,const MALTFormat::StackInfos & infos,const LocaltionMappingFunc & mapping) const;
		void buildTranslation(void);
		const std::string& getString(ssize_t id) const;
	private:
		const MALTFormat::MaltProfile & profile;
		std::map<MALTFormat::LangAddress, InstructionInfosStrRef> addrTranslation;
		std::string unknown{"??"};
};

/**********************************************************/
void to_json(nlohmann::json & json, const InstructionInfosStrRef & value);
void to_json(nlohmann::json & json, const FlatProfileValue & value);

}

#endif //MALT_READER_EXTRACTOR_HPP
