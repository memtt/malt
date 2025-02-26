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
void from_json(const nlohmann::json & json, ScatterPoint & value)
{
	//checks
	assert(json.contains("x"));
	assert(json.contains("y"));
	assert(json.contains("v"));

	//load
	json.at("x").get_to(value.x);
	json.at("y").get_to(value.y);
	json.at("v").get_to(value.v);
}

/**********************************************************/
void to_json(nlohmann::json & json, const ScatterChart & value)
{
	json = nlohmann::json{
		{"infos",
			{"xAxis",
				{"max", value.infos.xAxis.max},
				{"size", value.infos.xAxis.size},
				{"log", value.infos.xAxis.log},
			},
			{"yAxis",
				{"max", value.infos.yAxis.max},
				{"size", value.infos.yAxis.size},
				{"log", value.infos.yAxis.log},
			}
		},
		{"points", value.points}
	};
}

/**********************************************************/
void from_json(const nlohmann::json & json, ScatterChart & value)
{
	//checks
	assert(json.contains("infos"));
	assert(json.contains("points"));

	//load infos.xaxis
	const nlohmann::json & xAxis = json.at("infos").at("xAxis");
	assert(xAxis.contains("max"));
	assert(xAxis.contains("size"));
	assert(xAxis.contains("log"));
	xAxis.at("max").get_to(value.infos.xAxis.max);
	xAxis.at("size").get_to(value.infos.xAxis.size);
	xAxis.at("log").get_to(value.infos.xAxis.log);

	//load infos.yaxis
	const nlohmann::json & yAxis = json.at("infos").at("yAxis");
	assert(yAxis.contains("max"));
	assert(yAxis.contains("size"));
	assert(yAxis.contains("log"));
	xAxis.at("max").get_to(value.infos.yAxis.max);
	xAxis.at("size").get_to(value.infos.yAxis.size);
	xAxis.at("log").get_to(value.infos.yAxis.log);

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
void from_json(const nlohmann::json & json, Scatter & value)
{
	//checks
	assert(json.contains("sizeOverTime"));
	assert(json.contains("lifetimeOverSize"));

	//load
	json.at("sizeOverTime").get_to(value.sizeOverTime);
	json.at("lifetimeOverSize").get_to(value.lifetimeOverSize);
}

}
