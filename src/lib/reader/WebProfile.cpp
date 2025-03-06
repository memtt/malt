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

}

