/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MATT_MEMORY_TIMELINE_HPP
#define MATT_MEMORY_TIMELINE_HPP

/********************  HEADERS  *********************/
#include <cstdlib>
#include <cycle.h>
#include <json/ConvertToJson.h>

namespace MATT
{

/********************  MACROS  **********************/
#define MATT_PROFILED_VALUE_DEFAULT_STEPS 1024

/********************  STRUCT  **********************/
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
};

/*********************  CLASS  **********************/
class ProfiledStateValue
{
	public:
		ProfiledStateValue(size_t steps = MATT_PROFILED_VALUE_DEFAULT_STEPS, bool useLinearIndex = false);
		void onDeltaEvent(ssize_t delta);
		void onUpdateValue(size_t value);
		bool isNextPoint(void) const;
		void setRemoteLinearIndex(ticks * remoteLinearIndex);
	public:
		friend void convertToJson(htopml::JsonState& json, const ProfiledStateValue& value);
	private:
		inline void updateCurrentMinMax(ticks index, ticks timestamp);
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
};

}

#endif //MATT_MEMORY_TIMELINE_HPP
