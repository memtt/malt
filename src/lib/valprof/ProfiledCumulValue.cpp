/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
#include <json/JsonState.h>
#include "ProfiledCumulValue.hpp"

namespace MATT
{

/*******************  FUNCTION  *********************/
ProfiledCumulValue::ProfiledCumulValue(size_t steps,bool linearIndex)
{
	this->steps = steps;
	this->lastSeenTime = 0;
	this->entries = new size_t[steps];
	this->oldEntries = new size_t[steps];
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
ProfiledCumulValue::~ProfiledCumulValue(void)
{
	if (entries != NULL)
		delete[] entries;
	if (oldEntries != NULL)
		delete[] oldEntries;
	
	entries = NULL;
	oldEntries = NULL;
}

/*******************  FUNCTION  *********************/
void ProfiledCumulValue::push(size_t delta)
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
ticks ProfiledCumulValue::getIndex()
{
	ticks cur = eventId;
	eventId++;
	if (linearIndex)
		return cur;
	else
		return getticks();
}

/*******************  FUNCTION  *********************/
void ProfiledCumulValue::reshape(ticks neededIndex)
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
	size_t * tmp = entries;
	entries = oldEntries;
	oldEntries = tmp;
	memset(this->entries,0,sizeof(size_t) * steps);
	
	//recompute values
	for (size_t i = 0 ; i < steps ; i++)
		entries[i/ratio] += oldEntries[i];
	
	this->ticksPerEntry = newTickPerEntry;
}

/*******************  FUNCTION  *********************/
void convertToJson(htopml::JsonState& json, const ProfiledCumulValue& value)
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
