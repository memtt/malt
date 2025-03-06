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
};

}

#endif //MALT_READER_PROFILE_HPP
