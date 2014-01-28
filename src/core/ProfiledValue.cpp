/********************  HEADERS  *********************/
#include <cassert>
#include <cstring>
#include "ProfiledValue.hpp"

namespace ATT
{

/*******************  FUNCTION  *********************/
PorfiledValueEntry::PorfiledValueEntry(void)
{
	this->reset();
}

/*******************  FUNCTION  *********************/
void PorfiledValueEntry::reset(void)
{
	this->min = -1;
	this->max = 0;
	this->timestamp = 0;
}

/*******************  FUNCTION  *********************/
void PorfiledValueEntry::reduce(const PorfiledValueEntry& value)
{
	if (value.timestamp > timestamp)
		timestamp = value.timestamp;
	if (value.max > max)
		max = value.max;
	if (value.min < min)
		min = value.min;
}

/*******************  FUNCTION  *********************/
ProfiledValue::ProfiledValue(size_t steps,bool useLinearIndex)
{
	//errors
	assert(steps > 2); 

	//init pseudo clock for getticks and select mode
	this->linearIndex = 0;
	this->useLinearIndex = useLinearIndex;
	
	///setup vars
	this->startTime = getticks();
	this->currentId = 0;
	this->steps = steps;
	this->value = 0;
	
	//current
	this->deltaT = 1;
	this->nextT = startTime + deltaT;
	
	//memory
	this->entries = new PorfiledValueEntry[steps];
	memset(this->entries,0,sizeof(PorfiledValueEntry)*steps);
	
	//peak
	this->peak.min = 0;
	this->peak.max = 0;
	this->peak.timestamp = this->startTime;
}

/*******************  FUNCTION  *********************/
void ProfiledValue::onDeltaEvent(ssize_t delta)
{
	this->value += delta;
	onUpdateValue(value);
}

/*******************  FUNCTION  *********************/
void ProfiledValue::onUpdateValue(size_t value)
{
	//get current
	ticks t = getticks();
	this->value = value;
	
	//flush previous if really old
	if (t > nextT)
		this->flush();
	
	//update current
	updateCurrentMinMax(t);
	
	//update linear index
	linearIndex++;
}

/*******************  FUNCTION  *********************/
void ProfiledValue::updateCurrentMinMax(ticks t )
{
	//update current interval min/max
	if (value > current.max)
		this->current.max = value;
	if (value < current.min)
		this->current.min = value;
	this->current.timestamp = t;

	//update max
	if (this->value > this->peak.max)
	{
		this->peak.timestamp = t;
		this->peak.max = this->value;
	}
}

/*******************  FUNCTION  *********************/
void ProfiledValue::flush(void )
{
	//check if current if empty
	if (this->current.timestamp == 0)
		return;
	
	//check overflow
	if (currentId >= steps)
		resize();
	
	//fill
	this->entries[currentId++] = current;

	//reset current
	this->current.reset();
	
	//update next t
	this->nextT += deltaT;
}

/*******************  FUNCTION  *********************/
void ProfiledValue::resize(void )
{
	//errors
	assert(currentId == steps);
	
	//calc new delta t
	deltaT = (getticks() - startTime)/steps;
	
	//merge points
	int outId = 0;
	PorfiledValueEntry tmp = entries[0];
	for (int i = 1 ; i < steps ; i++)
	{
		if (entries[i].timestamp > startTime + (outId + 1) * deltaT && tmp.timestamp != 0)
		{
			entries[outId++] = tmp;
			tmp.reset();
		}
		tmp.reduce(entries[i]);
	}

	//update currentId and nextT
	this->currentId = outId;
	this->nextT = getticks() + deltaT;
}

/*******************  FUNCTION  *********************/
void typeToJson(htopml::JsonState& json, std::ostream& stream, const ProfiledValue& value)
{
	json.openStruct();
	
	json.openFieldArray("min");
	for (int i = 0 ; i < value.currentId ; i++)
		json.printValue(value.entries[i].min);
	json.closeFieldArray("min");
	
	json.openFieldArray("max");
	for (int i = 0 ; i < value.currentId ; i++)
		json.printValue(value.entries[i].max);
	json.closeFieldArray("max");
	
	json.openFieldArray("timestamp");
	for (int i = 0 ; i < value.currentId ; i++)
	{
		assert(value.entries[i].timestamp > value.startTime);
		json.printValue(value.entries[i].timestamp - value.startTime);
	}
	json.closeFieldArray("timestamp");
	
	json.printField("peakMemory",value.peak.max);
	assert(value.peak.timestamp > value.startTime);
	json.printField("peakTimesteamp",value.peak.timestamp - value.startTime);
	
	json.printField("linearIndex",value.useLinearIndex?"true":"false");
	json.closeStruct();
}

/*******************  FUNCTION  *********************/
ticks ProfiledValue::getticks()
{
	if (useLinearIndex)
		return linearIndex;
	else
		return ::getticks();
}


}
