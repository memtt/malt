#ifndef ATT_MEMORY_TIMELINE_HPP
#define ATT_MEMORY_TIMELINE_HPP

/********************  HEADERS  *********************/
#include <cstdlib>
#include <cycle.h>
#include <json/TypeToJson.h>

namespace ATT
{

/********************  MACROS  **********************/
#define ATT_PROFILED_VALUE_DEFAULT_STEPS 1024

/********************  STRUCT  **********************/
struct PorfiledValueEntry
{
	//funcs
	PorfiledValueEntry(void);
	void reduce(const PorfiledValueEntry & value);
	void reset(void);

	//values
	size_t min;
	size_t max;
	ticks timestamp;
};

/*********************  CLASS  **********************/
class ProfiledValue
{
	public:
		ProfiledValue(size_t steps = ATT_PROFILED_VALUE_DEFAULT_STEPS, bool useLinearIndex = false);
		void onDeltaEvent(ssize_t delta);
		void onUpdateValue(size_t value);
	public:
		friend void typeToJson(htopml::JsonState& json, std::ostream& stream, const ProfiledValue& value);
	private:
		void updateCurrentMinMax(ticks t);
		void flush(void);
		void resize(void);
		ticks getticks();
	private:
		size_t steps;
		ticks startTime;
		PorfiledValueEntry * entries;
		PorfiledValueEntry peak;
		PorfiledValueEntry current;
		size_t currentId;
		ticks deltaT;
		ticks nextT;
		size_t value;
		ticks linearIndex;
		bool useLinearIndex;
};

};

#endif //ATT_MEMORY_TIMELINE_HPP
