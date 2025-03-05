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
#include "Timeline.hpp"

/**********************************************************/
namespace MALTFormat
{

/**********************************************************/
void to_json(nlohmann::json & json, const TimelineInfos & value)
{
	json = nlohmann::json{
		{"start", value.start},
		{"fields", value.fields},
		{"perPoints", value.perPoints},
		{"peak", value.peak},
		{"values", value.values},
		{"callsite", value.callsite},
	};
}

/**********************************************************/
void from_json(const nlohmann::json & json, TimelineInfos & value)
{
	//checks
	assert(json.contains("start"));
	assert(json.contains("fields"));
	assert(json.contains("perPoints"));
	assert(json.contains("peak"));
	assert(json.contains("values"));
	assert(json.contains("callsite"));

	//load
	json.at("start").get_to(value.start);
	json.at("fields").get_to(value.fields);
	json.at("perPoints").get_to(value.perPoints);
	json.at("peak").get_to(value.peak);
	json.at("values").get_to(value.values);
	json.at("callsite").get_to(value.callsite);
}

/**********************************************************/
void to_json(nlohmann::json & json, const Timeline & value)
{
	json = nlohmann::json{
		{"memoryTimeline", value.memoryTimeline},
		{"systemTimeline", value.systemTimeline},
		{"memoryBandwidth", value.memoryBandwidth},
	};
}

/**********************************************************/
void from_json(const nlohmann::json & json, Timeline & value)
{
	//checks
	assert(json.contains("memoryTimeline"));
	assert(json.contains("systemTimeline"));

	//load
	json.at("memoryTimeline").get_to(value.memoryTimeline);
	json.at("systemTimeline").get_to(value.systemTimeline);
	json.at("memoryBandwidth").get_to(value.memoryBandwidth);
}

}
