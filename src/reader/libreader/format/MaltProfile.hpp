/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 07/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/reader/libreader/format/MaltProfile.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

#ifndef MALT_FORMAT_MALT_PROFILE_HPP
#define MALT_FORMAT_MALT_PROFILE_HPP

/**********************************************************/
#include <string>
#include "Run.hpp"
#include "Config.hpp"
#include "Stacks.hpp"
#include "Sites.hpp"
#include "Timeline.hpp"
#include "Scatter.hpp"
#include "Threads.hpp"
#include "MemStats.hpp"
#include "Globals.hpp"
#include "Leaks.hpp"
#include "Domains.hpp"

/**********************************************************/
namespace MALTFormat
{

/**********************************************************/
struct MaltProfile
{
	Run run;
	Config config;
	Stacks stacks;
	Sites sites;
	Timeline timeline;
	Scatter scatter;
	Threads threads;
	MemStats memStats;
	Globals globals;
	Leaks leaks;
	Domains domains;
};

/**********************************************************/
void from_json(const JsonIn & json, MaltProfile & value);
void to_json(nlohmann::json & json, const MaltProfile & value);

}

#endif //MALT_FORMAT_MALT_PROFILE_HPP
