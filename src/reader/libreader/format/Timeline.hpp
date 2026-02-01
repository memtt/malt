/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 03/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/reader/libreader/format/Timeline.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

#ifndef MALT_FORMAT_TIMELINE_HPP
#define MALT_FORMAT_TIMELINE_HPP

/**********************************************************/
#include <string>
#include <vector>
#include "Json.hpp"
#include "Types.hpp"
#include "Sites.hpp"

/**********************************************************/
namespace MALTFormat
{

/**********************************************************/
struct TimelineInfos
{
	CyclesTime start;
	std::vector<std::string> fields;
	CyclesDuration perPoints;
	std::vector<size_t> peak;
	std::vector< std::vector<size_t> > values;
	std::vector<LangAddress> callsite;
};

/**********************************************************/
struct Timeline
{
	TimelineInfos memoryTimeline;
	TimelineInfos systemTimeline;
	TimelineInfos memoryBandwidth;
	TimelineInfos memoryGpuBandwidth;
};

/**********************************************************/
void from_json(const JsonIn & json, TimelineInfos & value);
void to_json(nlohmann::json & json, const TimelineInfos & value);
void from_json(const JsonIn & json, Timeline & value);
void to_json(nlohmann::json & json, const Timeline & value);


}

#endif //MALT_FORMAT_TIMELINE_HPP
