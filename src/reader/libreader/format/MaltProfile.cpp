/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 10/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/reader/libreader/format/MaltProfile.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
#include "MaltProfile.hpp"

/**********************************************************/
namespace MALTFormat
{

/**********************************************************/
void to_json(nlohmann::json & json, const MaltProfile & value)
{
	json = nlohmann::json{
		{"run", value.run},
		{"config", value.config},
		{"stacks", value.stacks},
		{"sites", value.sites},
		{"timeline", value.timeline},
		{"scatter", value.scatter},
		{"threads", value.threads},
		{"memStats", value.memStats},
		{"globals", value.globals},
		{"leaks", value.leaks},
		{"domains", value.domains},
	};
}

/**********************************************************/
void from_json(const JsonIn & json, MaltProfile & profile)
{
	//check keys
	assert(jsContains(json, "run"));
	assert(jsContains(json, "config"));
	assert(jsContains(json, "stacks"));
	assert(jsContains(json, "sites"));
	assert(jsContains(json, "timeline"));
	assert(jsContains(json, "scatter"));
	assert(jsContains(json, "threads"));
	assert(jsContains(json, "globals"));
	assert(jsContains(json, "leaks"));
	assert(jsContains(json, "domains"));
	//assert(json.size() == 9);

	//load
	#pragma omp parallel
	{
		#pragma omp single
		{
			#pragma omp task
			json["run"].get_to(profile.run);
			#pragma omp task
			json["config"].get_to(profile.config);
			if (json.contains("stacks")) {
				#pragma omp task
				json["stacks"].get_to(profile.stacks);
			}
			#pragma omp task
			json["sites"].get_to(profile.sites);
			#pragma omp task
			json["timeline"].get_to(profile.timeline);
			#pragma omp task
			json["scatter"].get_to(profile.scatter);
			#pragma omp task
			json["threads"].get_to(profile.threads);
			#pragma omp task
			json["memStats"].get_to(profile.memStats);
			#pragma omp task
			json["globals"].get_to(profile.globals);
			#pragma omp task
			json["leaks"].get_to(profile.leaks);
			#pragma omp task
			json["domains"].get_to(profile.domains);
		}
	}
}

}
