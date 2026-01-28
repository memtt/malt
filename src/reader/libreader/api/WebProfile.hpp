/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 01/2026
*    LICENSE  : CeCILL-C
*    FILE     : src/reader/libreader/api/WebProfile.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
*    AUTHOR   : Sébastien Valat - 2026
***********************************************************/

#ifndef MALT_READER_WEB_PROFILE_HPP
#define MALT_READER_WEB_PROFILE_HPP

/**********************************************************/
#include "Profile.hpp"
#include "libinstrum/portability/Visibility.hpp"

/**********************************************************/
namespace MALTReader
{

/**********************************************************/
class WebProfile : public Profile
{
	public:
		DLL_PUBLIC WebProfile(const std::string & fname, bool loadProgressBar = false);
		DLL_PUBLIC ~WebProfile(void);
		DLL_PUBLIC nlohmann::json getProcMap(void) const;
		DLL_PUBLIC nlohmann::json getDebugStackList(void) const;
		DLL_PUBLIC nlohmann::json getGlobalVariables(void) const;
		DLL_PUBLIC nlohmann::json getFileLinesFlatProfile(const std::string & file, bool total) const;
		DLL_PUBLIC nlohmann::json getBinaryAddressesFlatProfile(const std::string & binaryFile, const std::vector<size_t> & offsets, bool total) const;
		DLL_PUBLIC nlohmann::json getBinaryAddressesFlatProfileAll(const std::string & binaryFile, bool total) const;
		DLL_PUBLIC nlohmann::json getFlatFunctionProfile(bool own, bool total) const;
		DLL_PUBLIC nlohmann::json getProcMapDistr(void) const;
		DLL_PUBLIC nlohmann::json getScatter(void) const;
		DLL_PUBLIC nlohmann::json getSizeMap(void) const;
		DLL_PUBLIC nlohmann::json getReallocMap(void) const;
		DLL_PUBLIC nlohmann::json getTimedValues(void) const;
		DLL_PUBLIC nlohmann::json getSummaryV2(void) const;
		DLL_PUBLIC nlohmann::json getSummary(void) const;
		DLL_PUBLIC nlohmann::json getFilterdStacksOnFileLine(const std::string & file, size_t line) const;
		DLL_PUBLIC nlohmann::json getFilterdStacksOnSymbol(const std::string & func) const;
		DLL_PUBLIC nlohmann::json getCallStackNextLevel(size_t parentStackId, size_t parentDepth, const LocationFilter & filter) const;
		DLL_PUBLIC nlohmann::json getStacksMem(void) const;
		DLL_PUBLIC nlohmann::json getStackInfoOnFunction(size_t thread_id) const;
		DLL_PUBLIC nlohmann::json getCallTree(ssize_t nodeId, ssize_t depth, ssize_t height, double minCost, const std::string & func, const std::string & metric, bool isRatio);
		DLL_PUBLIC nlohmann::json getStatic(void) const;
		DLL_PUBLIC nlohmann::json getStaticSummary(void) const;
		DLL_PUBLIC bool isSourceFile(const std::string & path) const;
		DLL_PUBLIC SourceFileMap getSourceFileMap(void) const;
	private:
		SourceFileMap sourceFiles;
		mutable nlohmann::json cachedFlatFunctionProfileTrueTrue;
};

}

#endif //MALT_READER_PROFILE_HPP
