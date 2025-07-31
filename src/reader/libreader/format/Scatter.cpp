/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.3.1
*    DATE     : 03/2025
*    LICENSE  : CeCILL-C
*    FILE     : ./src/reader/libreader/format/Scatter.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
#include "Scatter.hpp"

/**********************************************************/
namespace MALTFormat
{

/**********************************************************/
void to_json(nlohmann::json & json, const ScatterPoint & value)
{
	json = nlohmann::json{
		{"x", value.x},
		{"y", value.y},
		{"v", value.v},
	};
}

/**********************************************************/
void from_json(const JsonIn & json, ScatterPoint & value)
{
	//checks
	assert(jsContains(json, "x"));
	assert(jsContains(json, "y"));
	assert(jsContains(json, "v"));

	//load
	json.at("x").get_to(value.x);
	json.at("y").get_to(value.y);
	json.at("v").get_to(value.v);
}

/**********************************************************/
void to_json(nlohmann::json & json, const ScatterChart & value)
{
	json["infos"]["xAxis"] = nlohmann::json{
		{"max", value.infos.xAxis.max},
		{"size", value.infos.xAxis.size},
		{"log", value.infos.xAxis.log}
	};
	json["infos"]["yAxis"] = nlohmann::json{
		{"max", value.infos.yAxis.max},
		{"size", value.infos.yAxis.size},
		{"log", value.infos.yAxis.log}
	};
	json["points"] = value.points;
}

/**********************************************************/
void from_json(const JsonIn & json, ScatterChart & value)
{
	//checks
	assert(jsContains(json, "infos"));
	assert(jsContains(json, "points"));

	//load infos.xaxis
	const JsonIn & xAxis = json.at("infos").at("xAxis");
	assert(jsContains(xAxis, "max"));
	assert(jsContains(xAxis, "size"));
	assert(jsContains(xAxis, "log"));
	xAxis.at("max").get_to(value.infos.xAxis.max);
	xAxis.at("size").get_to(value.infos.xAxis.size);
	xAxis.at("log").get_to(value.infos.xAxis.log);

	//load infos.yaxis
	const JsonIn & yAxis = json.at("infos").at("yAxis");
	assert(jsContains(yAxis, "max"));
	assert(jsContains(yAxis, "size"));
	assert(jsContains(yAxis, "log"));
	yAxis.at("max").get_to(value.infos.yAxis.max);
	yAxis.at("size").get_to(value.infos.yAxis.size);
	yAxis.at("log").get_to(value.infos.yAxis.log);

	//load points
	json.at("points").get_to(value.points);
}

/**********************************************************/
void to_json(nlohmann::json & json, const Scatter & value)
{
	json = nlohmann::json{
		{"sizeOverTime", value.sizeOverTime},
		{"lifetimeOverSize", value.lifetimeOverSize},
	};
}

/**********************************************************/
void from_json(const JsonIn & json, Scatter & value)
{
	//checks
	assert(jsContains(json, "sizeOverTime"));
	assert(jsContains(json, "lifetimeOverSize"));

	//load
	json.at("sizeOverTime").get_to(value.sizeOverTime);
	json.at("lifetimeOverSize").get_to(value.lifetimeOverSize);
}

}
