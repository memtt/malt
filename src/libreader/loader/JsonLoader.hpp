/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 10/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/tools/Backtrace.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

#ifndef MALT_READER_JSON_LOADER_HPP
#define MALT_READER_JSON_LOADER_HPP

/**********************************************************/
#include "format/MaltProfile.hpp"
#include <nlohmann/json.hpp>

/**********************************************************/
namespace MALTReader
{

/**********************************************************/
class JsonLoader
{
	public:
		JsonLoader(const std::string & fname);
		~JsonLoader(void);
		MALTFormat::MaltProfile & getProfile(void);
		const MALTFormat::MaltProfile & getProfile(void) const;
	private:
		void loadFile(const std::string & fname);
		static void load(MALTFormat::MaltProfile & profile, const nlohmann::json & json);
		static void load(MALTFormat::Run & run, const nlohmann::json & json);
		static void load(MALTFormat::Config & config, const nlohmann::json & json);
		static void load(MALTFormat::Stacks & stacks, const nlohmann::json & json);
		static void load(MALTFormat::StackStats & stats, const nlohmann::json & json);
		static void load(MALTFormat::LangAddress & address, const nlohmann::json & json);
		static void load(MALTFormat::CountMinMaxSum & minMaxSum, const nlohmann::json & json);
		static void load(MALTFormat::StackInfos & stackInfos, const nlohmann::json & json);
		static void load(MALTFormat::ProcMapEntry & procMapEntry, const nlohmann::json & json);
		static void load(MALTFormat::InstructionInfos & instrInfos, const nlohmann::json & json);
		static void load(MALTFormat::Sites & sites, const nlohmann::json & json);
		static void load(MALTFormat::CallSite & sites, const nlohmann::json & json);
		static void load(MALTFormat::TimelineInfos& sites, const nlohmann::json & json);
		static void load(MALTFormat::Timeline& timeline, const nlohmann::json & json);
		static void load(void* & ptr, const nlohmann::json & json);
	private:
		MALTFormat::MaltProfile profile;
};

}

#endif //MALT_READER_JSON_LOADER_HPP
