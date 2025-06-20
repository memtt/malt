/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.3.1
*    DATE     : 02/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/valprof/ProfiledStateValue.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (ECR) - 2014
*    AUTHOR   : Sébastien Valat - 2014
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

#ifndef MALT_MEMORY_TIMELINE_HPP
#define MALT_MEMORY_TIMELINE_HPP

/**********************************************************/
#include <cstdlib>
#include <portability/Clock.hpp>
#include <json/ConvertToJson.h>
#include <stacks/LangAddress.hpp>

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
	LangAddress location;
};

/**********************************************************/
class ProfiledStateValue
{
	public:
		ProfiledStateValue(size_t steps = MALT_PROFILED_VALUE_DEFAULT_STEPS, bool useLinearIndex = false);
		~ProfiledStateValue(void);
		void onDeltaEvent(ssize_t delta, LangAddress location);
		void onUpdateValue(size_t value, LangAddress location);
		bool isNextPoint(void) const;
		void setRemoteLinearIndex(ticks * remoteLinearIndex);
		void disableTimestamp(void);
	public:
		friend void convertToJson(htopml::JsonState& json, const ProfiledStateValue& value);
	private:
		inline void updateCurrentMinMax(ticks index, ticks timestamp, LangAddress location);
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
