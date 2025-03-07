/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 10/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/tools/Backtrace.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

#ifndef MALT_FORMAT_TIMELINE_HPP
#define MALT_FORMAT_TIMELINE_HPP

/**********************************************************/
#include <string>
#include <vector>
#include "Types.hpp"
#include "Sites.hpp"

/**********************************************************/
namespace MALTFormat
{

/**********************************************************/
struct CallSite
{
	LangAddress orig;
	const InstructionInfos * translated;
};

/**********************************************************/
struct TimelineInfos
{
	CyclesTime start;
	std::vector<std::string> fields;
	CyclesDuration perPoints;
	std::vector<size_t> peak;
	std::vector< std::vector<size_t> > values;
	std::vector<CallSite> callsite;
};

/**********************************************************/
struct Timeline
{
	TimelineInfos memoryTimeline;
	TimelineInfos systemTimeline;
	TimelineInfos memoryBandwidth;
};

}

#endif //MALT_FORMAT_TIMELINE_HPP
