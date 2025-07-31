/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 10/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/tools/Backtrace.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

#ifndef MALT_READER_EXTRACTOR_HELPER_HPP
#define MALT_READER_EXTRACTOR_HELPER_HPP

/**********************************************************/
#include <queue>
#include <nlohmann/json.hpp>
#include "../format/MaltProfile.hpp"

/**********************************************************/
namespace MALTReader
{

/**********************************************************/
struct ExtractorHelpers
{
	static bool isAllocFunction(const std::vector<std::string> & allocatorWrappers, const std::string & name);
	template <class T>
	static nlohmann::json toJsonFiltered(const T & value, const std::vector<std::string> & fieldsToKeep);
	static void filterJson(nlohmann::json & value, const std::vector<std::string> & fieldsToKeep);
	static bool filterJson(nlohmann::json & value, const std::vector<std::string> & fieldsToKeep, std::vector<std::string> & stack);
	static bool jsonCheckPath(const std::vector<std::string> & fieldsToKeep, const std::vector<std::string> & stack);
	static void jsonRemoveAbsPath(nlohmann::json & value, const std::string & prefix);
	static bool jsonRemoveZeroes(nlohmann::json & value);
	static nlohmann::json buildShorterFlatProfileSummary(nlohmann::json value, bool onlyLocation = false);
	static std::string humanReadable(double value,int decimals,const std::string & unit, bool protectedSpace = false);
};

/**********************************************************/
template <class T>
nlohmann::json ExtractorHelpers::toJsonFiltered(const T & value, const std::vector<std::string> & fieldsToKeep)
{
	//convert full
	nlohmann::json json = value;

	//filter
	filterJson(json, fieldsToKeep);

	//ok
	return json;
}

}

#endif //MALT_READER_EXTRACTOR_HELPER_HPP
