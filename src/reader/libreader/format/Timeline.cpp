/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 03/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/reader/libreader/format/Timeline.cpp
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
void from_json(const JsonIn & json, TimelineInfos & value)
{
	//checks
	assert(jsContains(json, "start"));
	assert(jsContains(json, "fields"));
	assert(jsContains(json, "perPoints"));
	assert(jsContains(json, "peak"));
	assert(jsContains(json, "values"));
	assert(jsContains(json, "callsite"));

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
		{"memoryGpuBandwidth", value.memoryGpuBandwidth},
	};
}

/**********************************************************/
void from_json(const JsonIn & json, Timeline & value)
{
	//checks
	assert(jsContains(json, "memoryTimeline"));
	assert(jsContains(json, "systemTimeline"));

	//load
	json.at("memoryTimeline").get_to(value.memoryTimeline);
	json.at("systemTimeline").get_to(value.systemTimeline);
	json.at("memoryBandwidth").get_to(value.memoryBandwidth);
	json.at("memoryGpuBandwidth").get_to(value.memoryGpuBandwidth);
}

}
