/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MATT_PROFILED_CUMUL_VALUE_HPP
#define MATT_PROFILED_CUMUL_VALUE_HPP

/********************  HEADERS  *********************/
#include <cstdlib>
#include <cycle.h>
#include <json/ConvertToJson.h>

namespace MATT
{

/********************  MACROS  **********************/
#define MATT_PROFILED_CUMUL_VALUE_DEFAULT_STEPS 1024

/*********************  CLASS  **********************/
class ProfiledCumulValue
{
	public:
		ProfiledCumulValue(size_t steps = MATT_PROFILED_CUMUL_VALUE_DEFAULT_STEPS,bool linearIndex = false);
		~ProfiledCumulValue(void);
		void push(size_t delta);
	public:
		friend void convertToJson(htopml::JsonState& json, const ProfiledCumulValue& value);
	private:
		void reshape(ticks neededIndex);
		ticks getIndex();
	private:
		size_t steps;
		ticks startTime;
		ticks lastSeenTime;
		size_t * entries;
		size_t * oldEntries;
		ticks ticksPerEntry;
		size_t eventId;
		bool linearIndex;
};

}

#endif //MATT_PROFILED_CUMUL_VALUE_HPP
