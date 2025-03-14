/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.6
*    DATE     : 09/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/valprof/ProfiledStateValue.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2014 - 2024
*    AUTHOR   : Sébastien Valat (ECR) - 2014
***********************************************************/

#ifndef MALT_MEMORY_TIMELINE_HPP
#define MALT_MEMORY_TIMELINE_HPP

/**********************************************************/
#include <cstdlib>
#include <cycle.h>
#include <json/ConvertToJson.h>

namespace MALT
{

/**********************************************************/
#define MALT_PROFILED_VALUE_DEFAULT_STEPS 1024

/**********************************************************/
struct ProfiledStateValueEntry
{
	//funcs
	ProfiledStateValueEntry(void);
	void reduce(const ProfiledStateValueEntry & value);
	void reset(void);

	//values
	size_t min;
	size_t max;
	ticks index;
	ticks timestamp;
	void * location;
};

/**********************************************************/
class ProfiledStateValue
{
	public:
		ProfiledStateValue(size_t steps = MALT_PROFILED_VALUE_DEFAULT_STEPS, bool useLinearIndex = false);
		void onDeltaEvent(ssize_t delta, void* location);
		void onUpdateValue(size_t value, void* location);
		bool isNextPoint(void) const;
		void setRemoteLinearIndex(ticks * remoteLinearIndex);
		void disableTimestamp(void);
	public:
		friend void convertToJson(htopml::JsonState& json, const ProfiledStateValue& value);
	private:
		inline void updateCurrentMinMax(ticks index, ticks timestamp, void* location);
		void flush(void);
		void resize(void);
		inline ticks getIndex() const;
	private:
		size_t steps;
		ticks startTime;
		ticks startIndex;
		ProfiledStateValueEntry * entries;
		ProfiledStateValueEntry peak;
		ProfiledStateValueEntry current;
		size_t currentId;
		ticks deltaIndex;
		ticks nextIndex;
		size_t value;
		ticks linearIndex;
		bool useLinearIndex;
		ticks * remoteLinearIndex;
		bool printTimestamps;
		bool hasLocation;
};

}

#endif //MALT_MEMORY_TIMELINE_HPP
