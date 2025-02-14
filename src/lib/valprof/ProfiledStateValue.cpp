/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 10/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/valprof/ProfiledStateValue.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2014 - 2024
*    AUTHOR   : Sébastien Valat (ECR) - 2014
***********************************************************/

/**********************************************************/
#include <cassert>
#include <cstring>
#include "ProfiledStateValue.hpp"

namespace MALT
{

/**********************************************************/
ProfiledStateValueEntry::ProfiledStateValueEntry(void)
{
	this->reset();
}

/**********************************************************/
void ProfiledStateValueEntry::reset(void)
{
	this->min = -1;
	this->max = 0;
	this->index = 0;
	this->location = nullAddr;
}

/**********************************************************/
void ProfiledStateValueEntry::reduce(const ProfiledStateValueEntry& value)
{
	if (value.index > index)
		index = value.index;
	if (value.timestamp > timestamp)
	{
		timestamp = value.timestamp;
		location = value.location;
	}
	if (value.max > max)
		max = value.max;
	if (value.min < min)
		min = value.min;
}

/**********************************************************/
ProfiledStateValue::ProfiledStateValue(size_t steps,bool useLinearIndex)
{
	//errors
	assert(steps > 2); 

	//init pseudo clock for getticks and select mode
	this->linearIndex = 0;
	this->useLinearIndex = useLinearIndex;
	this->remoteLinearIndex = NULL;
	
	///setup vars
	this->startTime = Clock::getticks();
	this->startIndex = getIndex();
	this->currentId = 0;
	this->steps = steps;
	this->value = 0;
	this->hasLocation = false;
	
	//current
	this->deltaIndex = 1;
	this->nextIndex = startIndex + deltaIndex;
	
	//memory
	this->entries = new ProfiledStateValueEntry[steps];
	
	//peak
	this->peak.min = 0;
	this->peak.max = 0;
	this->peak.index = this->startIndex;
	this->peak.timestamp = this->startTime;
	this->printTimestamps = true;
}

/**********************************************************/
void ProfiledStateValue::onDeltaEvent(ssize_t delta,LangAddress location)
{
	this->value += delta;
	onUpdateValue(value,location);
}

/**********************************************************/
void ProfiledStateValue::onUpdateValue(size_t value,LangAddress location)
{
	//get current
	ticks index = getIndex();
	ticks timestamp = Clock::getticks();
	this->value = value;
	
	if (location.isNULL() == false)
		this->hasLocation = true;
	
	//flush previous if really old
	if (index > nextIndex)
		this->flush();
	
	//update current
	updateCurrentMinMax(index,timestamp, location);
	
	//update linear index
	linearIndex++;
}

/**********************************************************/
void ProfiledStateValue::updateCurrentMinMax(ticks index, ticks timestamp, LangAddress location )
{
	//update current interval min/max
	if (value > current.max)
		this->current.max = value;
	if (value < current.min)
		this->current.min = value;
	this->current.index = index;
	this->current.timestamp = timestamp;
	this->current.location = location;

	//update max
	if (this->value > this->peak.max)
	{
		this->peak.timestamp = timestamp;
		this->peak.index = index;
		this->peak.max = this->value;
	}
}

/**********************************************************/
bool ProfiledStateValue::isNextPoint(void) const
{
	return getIndex() > nextIndex;
}

/**********************************************************/
void ProfiledStateValue::flush(void )
{
	//check if current if empty
	if (this->current.index == 0)
		return;
	
	//check overflow
	if (currentId >= steps)
		resize();
	
	//fill
	this->entries[currentId++] = current;

	//reset current
	this->current.reset();
	
	//update next t
	this->nextIndex += deltaIndex;
}

/**********************************************************/
void ProfiledStateValue::resize(void )
{
	//errors
	assert(currentId == steps);
	
	//calc new delta t
	deltaIndex = 2*(getIndex() - startIndex)/steps;
	
	//merge points
	int outId = 0;
	ProfiledStateValueEntry tmp = entries[0];
	for (size_t i = 1 ; i < steps ; i++)
	{
		if (entries[i].index > startIndex + (outId + 1) * deltaIndex && tmp.index != 0)
		{
			entries[outId++] = tmp;
			tmp.reset();
		}
		tmp.reduce(entries[i]);
	}

	//update currentId and nextT
	this->currentId = outId;
	this->nextIndex = getIndex() + deltaIndex;
}

/**********************************************************/
void convertToJson(htopml::JsonState& json, const ProfiledStateValue& value)
{
	json.openStruct();
	
	json.openFieldArray("min");
	for (size_t i = 0 ; i < value.currentId ; i++)
		json.printValue(value.entries[i].min);
	json.closeFieldArray("min");
	
	json.openFieldArray("max");
	for (size_t i = 0 ; i < value.currentId ; i++)
		json.printValue(value.entries[i].max);
	json.closeFieldArray("max");
	
	json.openFieldArray("index");
	for (size_t i = 0 ; i < value.currentId ; i++)
	{
		assert(value.entries[i].index > value.startIndex);
		json.printValue(value.entries[i].index - value.startIndex);
	}
	json.closeFieldArray("index");
	
	if (value.hasLocation)
	{
		json.openFieldArray("location");
		for (size_t i = 0 ; i < value.currentId ; i++)
			json.printValue(value.entries[i].location);
		json.closeFieldArray("location");
	}
	
	if (value.printTimestamps)
	{
		json.openFieldArray("timestamp");
			for (size_t i = 0 ; i < value.currentId ; i++)
			{
				assert(value.entries[i].timestamp > value.startTime);
				json.printValue(value.entries[i].timestamp - value.startTime);
			}
		json.closeFieldArray("timestamp");
		json.printField("peakTimesteamp",value.peak.timestamp - value.startTime);
	}

	json.printField("peakMemory",value.peak.max);
	assert(value.peak.timestamp >= value.startTime);
	json.printField("peakIndex",value.peak.index - value.startIndex);
	
	json.printField("linearIndex",value.useLinearIndex);
	json.closeStruct();
}

/**********************************************************/
ticks ProfiledStateValue::getIndex() const
{
	if (useLinearIndex == false)
	{
		return Clock::getticks();
	} else if (remoteLinearIndex == NULL) {
		return linearIndex;
	} else {
		return *remoteLinearIndex;
	}
}

/**********************************************************/
void ProfiledStateValue::setRemoteLinearIndex(ticks* remoteLinearIndex)
{
	this->remoteLinearIndex = remoteLinearIndex;
}

/**********************************************************/
void ProfiledStateValue::disableTimestamp(void )
{
	this->printTimestamps = false;
}

}
