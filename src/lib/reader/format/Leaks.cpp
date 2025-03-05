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
#include "Leaks.hpp"

/**********************************************************/
namespace MALTFormat
{

/**********************************************************/
void to_json(nlohmann::json & json, const Leak & value)
{
	json = nlohmann::json{
		{"stack", value.stack},
		{"count", value.count},
		{"memory", value.memory},
	};
}

/**********************************************************/
void from_json(const nlohmann::json & json, Leak & value)
{
	//checks
	assert(json.contains("stack"));
	assert(json.contains("count"));
	assert(json.contains("memory"));

	//load
	json.at("stack").get_to(value.stack);
	json.at("count").get_to(value.count);
	json.at("memory").get_to(value.memory);
}

}
