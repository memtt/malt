/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 10/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/tools/Backtrace.hpp
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
		nlohmann::json functiongetReallocMap(void) const;
		nlohmann::json getTimedValues(void) const;
		nlohmann::json getSummaryV2(void) const;
		nlohmann::json getSummary(void) const;
		nlohmann::json getFilterdStacksOnFileLine(const std::string & file, size_t line) const;
		nlohmann::json getFilterdStacksOnSymbol(const std::string & func) const;
		nlohmann::json getCallStackNextLevel(size_t parentStackId, size_t parentDepth) const;
		bool isSourceFile(const std::string & path) const;
	private:
		SourceFileMap sourceFiles;
};

}

#endif //MALT_READER_PROFILE_HPP
