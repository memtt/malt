/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 10/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/tools/Backtrace.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

#ifndef MALT_FORMAT_SCATTER_HPP
#define MALT_FORMAT_SCATTER_HPP

/**********************************************************/
#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include "Types.hpp"

/**********************************************************/
namespace MALTFormat
{

/**********************************************************/
struct ScatterPoint
{
	size_t x;
	size_t y;
	size_t v;
};

/**********************************************************/
struct ScatterChart
{
	struct {
		struct {
			size_t max;
			size_t size;
			bool log;
		} xAxis;
		struct {
			size_t max;
			size_t size;
			bool log;
		} yAxis;
	} infos;
	std::vector<ScatterPoint> points;
};

/**********************************************************/
struct Scatter
{
	ScatterChart sizeOverTime;
	ScatterChart lifetimeOverSize;
};

/**********************************************************/
void from_json(const nlohmann::json & json, ScatterPoint & value);
void to_json(nlohmann::json & json, const ScatterPoint & value);
void from_json(const nlohmann::json & json, ScatterChart & value);
void to_json(nlohmann::json & json, const ScatterChart & value);
void from_json(const nlohmann::json & json, Scatter & value);
void to_json(nlohmann::json & json, const Scatter & value);

}

#endif //MALT_FORMAT_SCATTER_HPP
