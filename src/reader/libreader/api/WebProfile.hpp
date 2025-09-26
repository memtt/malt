/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 06/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/reader/libreader/api/WebProfile.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

#ifndef MALT_READER_WEB_PROFILE_HPP
#define MALT_READER_WEB_PROFILE_HPP

/**********************************************************/
#include "Profile.hpp"

/**********************************************************/
namespace MALTReader
{

/**********************************************************/
class WebProfile : public Profile
{
	public:
		WebProfile(const std::string & fname, bool loadProgressBar = false);
		~WebProfile(void);
		nlohmann::json getProcMap(void) const;
		nlohmann::json getDebugStackList(void) const;
		nlohmann::json getGlobalVariables(void) const;
		nlohmann::json getFileLinesFlatProfile(const std::string & file, bool total) const;
		nlohmann::json getFlatFunctionProfile(bool own, bool total) const;
		nlohmann::json getProcMapDistr(void) const;
		nlohmann::json getScatter(void) const;
		nlohmann::json getSizeMap(void) const;
		nlohmann::json getReallocMap(void) const;
		nlohmann::json getTimedValues(void) const;
		nlohmann::json getSummaryV2(void) const;
		nlohmann::json getSummary(void) const;
		nlohmann::json getFilterdStacksOnFileLine(const std::string & file, size_t line) const;
		nlohmann::json getFilterdStacksOnSymbol(const std::string & func) const;
		nlohmann::json getCallStackNextLevel(size_t parentStackId, size_t parentDepth, const LocationFilter & filter) const;
		nlohmann::json getStacksMem(void) const;
		nlohmann::json getStackInfoOnFunction(size_t thread_id) const;
		nlohmann::json getCallTree(ssize_t nodeId, ssize_t depth, ssize_t height, double minCost, const std::string & func, const std::string & metric, bool isRatio);
		bool isSourceFile(const std::string & path) const;
	private:
		SourceFileMap sourceFiles;
		mutable nlohmann::json cachedFlatFunctionProfileTrueTrue;
};

}

#endif //MALT_READER_PROFILE_HPP
