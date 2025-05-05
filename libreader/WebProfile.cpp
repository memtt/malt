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
#include "extractors/ExtractorHelpers.hpp"
#include "WebProfile.hpp"

/**********************************************************/
namespace MALTReader
{

/**********************************************************/
WebProfile::WebProfile(const std::string & fname, bool loadProgressBar)
	:Profile(fname, loadProgressBar)
{
}

/**********************************************************/
WebProfile::~WebProfile(void)
{
}

/**********************************************************/
nlohmann::json WebProfile::getProcMap(void) const
{
	return this->profile.sites.map;
}

/**********************************************************/
nlohmann::json WebProfile::getDebugStackList(void) const
{
	return this->extractor->getDebugStackList();
}

/**********************************************************/
nlohmann::json WebProfile::getGlobalVariables(void) const
{
	return nlohmann::json{
		{"vars", this->profile.memStats.globalVariables},
		{"maxThreadCount", this->profile.globals.maxThreadCount}
	};
}

/**********************************************************/
nlohmann::json WebProfile::getFileLinesFlatProfile(const std::string & file, bool total) const
{
	//extract
	FlatProfileVector res = this->extractor->getFlatProfile([](const InstructionInfosStrRef & location, const MALTFormat::StackInfos & infos){
		char buffer[256];
		snprintf(buffer, sizeof(buffer), "%zd", location.line);
		return std::string(buffer);
	},[&file](const InstructionInfosStrRef & location, const MALTFormat::StackInfos & infos){
		return *location.file == file;
	});

	//select
	std::vector<std::string> subset{
		"*.own",
		"*.line",
		"*.function"
	};
	if (total)
		subset.push_back("*.total");
	else
		subset.push_back("*.childs");

	//filter
	nlohmann::json resJson = ExtractorHelpers::toJsonFiltered(res, subset);

	//ok
	return resJson;
}

/**********************************************************/
nlohmann::json WebProfile::getFlatFunctionProfile(bool own, bool total) const
{
	//extract
	FlatProfileVector res = this->extractor->getFlatProfile([](const InstructionInfosStrRef & location, const MALTFormat::StackInfos & infos){
		char buffer[8192];
		snprintf(buffer, sizeof(buffer), "%s:%s", location.file->c_str(), location.function->c_str());
		return std::string(buffer);
	},[](const InstructionInfosStrRef & location, const MALTFormat::StackInfos & infos){
		return true;
	});

	//fields to keep
	std::vector<std::string> fieldsToKeep{
		/*"*.location.line",
		"*.location.function",
		"*.location.functionShort",
		"*.location.file",*/
		"*.line",
		"*.function",
		"*.functionShort",
		"*.file",
	};

	//select
	if (own)
		fieldsToKeep.push_back("*.own");
	if (total)
		fieldsToKeep.push_back("*.total");

	//filter
	nlohmann::json resJson = ExtractorHelpers::toJsonFiltered(res, fieldsToKeep);

	//ok
	return resJson;
}

/**********************************************************/
nlohmann::json WebProfile::getProcMapDistr(void) const
{
	ProcMapDistr res = this->extractor->getProcMapDistr();
	return res;
}

/**********************************************************/
nlohmann::json WebProfile::getScatter(void) const
{
	return this->profile.scatter;
}

/**********************************************************/
nlohmann::json WebProfile::getSizeMap(void) const
{
	return this->profile.memStats.sizeMap;
}

/**********************************************************/
nlohmann::json WebProfile::functiongetReallocMap(void) const
{
	return this->profile.memStats.reallocJump;
}

/**********************************************************/
nlohmann::json WebProfile::getTimedValues(void) const
{
	return this->extractor->getTimedValues();
}

/**********************************************************/
nlohmann::json WebProfile::getSummaryV2(void) const
{
	return this->extractor->getSummaryV2();
}

/**********************************************************/
nlohmann::json WebProfile::getSummary(void) const
{
	return this->extractor->getSummary();
}

/**********************************************************/
nlohmann::json WebProfile::getFilterdStacksOnFileLine(const std::string & file, size_t line) const
{
	return this->extractor->getFilterdStacksOnFileLine(file, line);
}

/**********************************************************/
nlohmann::json WebProfile::getFilterdStacksOnSymbol(const std::string & func) const
{
	return this->extractor->getFilterdStacksOnSymbol(func);
}

/**********************************************************/
nlohmann::json WebProfile::getCallStackNextLevel(size_t parentStackId, size_t parentDepth, const LocationFilter & filter) const
{
	return this->extractor->getCallStackNextLevel(parentStackId, parentDepth, filter);
}

/**********************************************************/
bool WebProfile::isSourceFile(const std::string & path) const
{
	const auto it = this->sourceFiles.find(path);
	return it != this->sourceFiles.end();
}

}

