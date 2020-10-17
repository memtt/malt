/*****************************************************
             PROJECT  : MALT
             VERSION  : 1.2.2
             DATE     : 10/2020
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MALT_PROFILED_CUMUL_VALUE_HPP
#define MALT_PROFILED_CUMUL_VALUE_HPP

/********************  HEADERS  *********************/
#include <cstdlib>
#include <cycle.h>
#include <json/ConvertToJson.h>

namespace MALT
{

/********************  MACROS  **********************/
#define MALT_PROFILED_CUMUL_VALUE_DEFAULT_STEPS 1024

/*********************  CLASS  **********************/
template <class T>
class ProfiledCumulValue
{
	public:
		ProfiledCumulValue(size_t steps = MALT_PROFILED_CUMUL_VALUE_DEFAULT_STEPS,bool linearIndex = false,bool initMemset = false);
		~ProfiledCumulValue(void);
		void push(const T & delta);
	public:
		template <class U> friend void convertToJson(htopml::JsonState& json, const ProfiledCumulValue<U> & value);
	private:
		void reshape(ticks neededIndex);
		ticks getIndex();
	private:
		size_t steps;
		ticks startTime;
		ticks lastSeenTime;
		T * entries;
		T * oldEntries;
		ticks ticksPerEntry;
		size_t eventId;
		bool linearIndex;
};

/*******************  FUNCTION  *********************/
template <class T>
ProfiledCumulValue<T>::ProfiledCumulValue(size_t steps,bool linearIndex,bool initMemset)
{
	this->steps = steps;
	this->lastSeenTime = 0;
	this->entries = new T[steps];
	this->oldEntries = new T[steps];
	if (initMemset)
		memset(this->entries,0,sizeof(size_t) * steps);
	this->ticksPerEntry = 1;
	this->eventId = 0;
	this->linearIndex = linearIndex;
	
	if (linearIndex)
		this->startTime = 0;
	else
		startTime = getticks();
}

/*******************  FUNCTION  *********************/
template <class T>
ProfiledCumulValue<T>::~ProfiledCumulValue(void)
{
	if (entries != NULL)
		delete[] entries;
	if (oldEntries != NULL)
		delete[] oldEntries;
	
	entries = NULL;
	oldEntries = NULL;
}

/*******************  FUNCTION  *********************/
template <class T>
void ProfiledCumulValue<T>::push(const T & delta)
{
	//get current index
	ticks cur = getIndex();
	if (cur >= startTime + ticksPerEntry * steps)
		reshape(cur);
	
	//find cell ID
	size_t id = (cur-startTime) / ticksPerEntry;
	assert(id >= 0 && id < steps);
	
	//last seen
	lastSeenTime = cur;
	
	//incr
	entries[id] += delta;
}

/*******************  FUNCTION  *********************/
template <class T>
ticks ProfiledCumulValue<T>::getIndex()
{
	ticks cur = eventId;
	eventId++;
	if (linearIndex)
		return cur;
	else
		return getticks();
}

/*******************  FUNCTION  *********************/
template <class T>
void ProfiledCumulValue<T>::reshape(ticks neededIndex)
{
	//find next scale
	ticks newTickPerEntry = ticksPerEntry;
	int ratio = 1;
	while (neededIndex - startTime >= newTickPerEntry * steps)
	{
		newTickPerEntry *= 2;
		ratio *= 2;
	}
	
	//check
	assert(ratio > 1);
	
	//permut arrays
	T * tmp = entries;
	entries = oldEntries;
	oldEntries = tmp;
	memset(this->entries,0,sizeof(size_t) * steps);
	
	//recompute values
	for (size_t i = 0 ; i < steps ; i++)
		entries[i/ratio] += oldEntries[i];
	
	this->ticksPerEntry = newTickPerEntry;
}

/*******************  FUNCTION  *********************/
template <class T>
void convertToJson(htopml::JsonState& json, const ProfiledCumulValue<T>& value)
{
	size_t cnt = 1 + (value.lastSeenTime-value.startTime) / value.ticksPerEntry;
	assert(cnt >= 0 && cnt <= value.steps);
	json.openStruct();

	json.printField("startTime",value.startTime);
	json.printField("scale",value.ticksPerEntry);
	json.printField("endTime",value.lastSeenTime);
	json.printFieldArray("values",value.entries,cnt);
	
	json.closeStruct();
}

}

#endif //MALT_PROFILED_CUMUL_VALUE_HPP
