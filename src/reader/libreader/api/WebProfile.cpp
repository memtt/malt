/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 10/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/reader/libreader/api/WebProfile.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
#include <iostream>
#include "extractors/ExtractorHelpers.hpp"
#include "WebProfile.hpp"

/**********************************************************/
using namespace MALTFormat;

/**********************************************************/
namespace MALTReader
{

/**********************************************************/
WebProfile::WebProfile(const std::string & fname, bool loadProgressBar)
	:Profile(fname, loadProgressBar)
{
	//pre compute and put in cache
	this->getFlatFunctionProfile(true, true);
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
nlohmann::json WebProfile::getBinaryAddressesFlatProfile(const std::string & binaryFile, const std::vector<size_t> & offsets, bool total) const
{
	//convert the offsets
	std::vector<size_t> virtualAddresses = offsets;
	this->extractor->toVirtualAddresses(virtualAddresses, binaryFile);

	//extract
	FlatProfileVector res = this->extractor->getFlatProfile([](const InstructionInfosStrRef & location, const MALTFormat::StackInfos & infos){
		char buffer[256];
		snprintf(buffer, sizeof(buffer), "%p", location.origin.address);
		return std::string(buffer);
	},[&virtualAddresses](const InstructionInfosStrRef & location, const MALTFormat::StackInfos & infos){
		return location.origin.lang == LANG_C &&  (std::find(virtualAddresses.begin(), virtualAddresses.end(), (size_t)location.origin.address) != virtualAddresses.end());
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
	//cached
	if (this->cachedFlatFunctionProfileTrueTrue.is_null() == false && own == true && total == true) {
		return this->cachedFlatFunctionProfileTrueTrue;
	}

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

	//cached
	if (this->cachedFlatFunctionProfileTrueTrue.is_null() == true && own == true && total == true) {
		this->cachedFlatFunctionProfileTrueTrue = resJson;
	}

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
	//convert
	nlohmann::json json = this->profile.scatter;

	//append
	json["ticksPerSecond"] = this->profile.globals.ticksPerSecond;

	//ok
	return json;
}

/**********************************************************/
nlohmann::json WebProfile::getSizeMap(void) const
{
	nlohmann::json js;
	MALTFormat::to_json(js, this->profile.memStats.sizeMap);
	return js;
}

/**********************************************************/
nlohmann::json WebProfile::getReallocMap(void) const
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
nlohmann::json WebProfile::getStacksMem(void) const
{
	return this->extractor->getStackMem();
}

/**********************************************************/
nlohmann::json WebProfile::getStackInfoOnFunction(size_t thread_id) const
{
	return this->extractor->getStackInfoOnFunction(thread_id);
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
nlohmann::json WebProfile::getCallTree(ssize_t nodeId, ssize_t depth, ssize_t height, double minCost, const std::string & func, const std::string & metric, bool isRatio)
{
	nlohmann::json data = this->extractor->getCallTree(nodeId, depth, height, minCost, func, metric, isRatio);
	//std::cout << data << std::endl;
	return data;
}

/**********************************************************/
bool WebProfile::isSourceFile(const std::string & path) const
{
	const auto it = this->sourceFiles.find(path);
	return it != this->sourceFiles.end();
}

/**********************************************************/
SourceFileMap WebProfile::getSourceFileMap(void) const
{
	return this->getExtractor().getSourceFileMap();
}

}

