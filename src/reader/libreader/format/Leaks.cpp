/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 07/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/reader/libreader/format/Leaks.cpp
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
void from_json(const JsonIn & json, Leak & value)
{
	//checks
	assert(jsContains(json, "stack"));
	assert(jsContains(json, "count"));
	assert(jsContains(json, "memory"));

	//load
	json.at("stack").get_to(value.stack);
	json.at("count").get_to(value.count);
	json.at("memory").get_to(value.memory);
}

}
