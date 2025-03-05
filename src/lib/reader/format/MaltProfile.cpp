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
	};
}

/**********************************************************/
void from_json(const nlohmann::json & json, MaltProfile & profile)
{
	//check keys
	assert(json.contains("run"));
	assert(json.contains("config"));
	assert(json.contains("stacks"));
	assert(json.contains("sites"));
	assert(json.contains("timeline"));
	assert(json.contains("scatter"));
	assert(json.contains("threads"));
	assert(json.contains("globals"));
	assert(json.contains("leaks"));
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
			#pragma omp task
			json["stacks"].get_to(profile.stacks);
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
		}
	}
}

}
