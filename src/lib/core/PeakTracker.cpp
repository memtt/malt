/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 02/2015
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
#include "PeakTracker.hpp"
#include <json/JsonState.h>

/********************  NAMESPACE  *******************/
namespace MATT
{

/*******************  FUNCTION  *********************/
PeakTracker::PeakTracker ( void )
{
	this->current = 0;
	this->peakId = 0;
	this->peakTime = 0;
	this->peakValue = 0;
}

/*******************  FUNCTION  *********************/
void PeakTracker::update ( ticks time, ssize_t delta )
{
	//update current
	this->current += delta;
	
	//update peak info
	if (this->current > this->peakValue)
	{
		this->peakId++;
		this->peakTime=time;
		this->peakValue = this->current;
	}
}

/*******************  FUNCTION  *********************/
void convertToJson ( htopml::JsonState& json, const PeakTracker& value )
{
	json.openStruct();
		json.printField("finalValue",value.current);
		json.printField("peakTime",value.peakTime);
		json.printField("peak",value.peakValue);
	json.closeStruct();
}

/*******************  FUNCTION  *********************/
StackPeakTracker::StackPeakTracker ( void ) : PeakTracker()
{
	this->onGlobalPeak = 0;
	this->lastGlobalPeakId = 0;
}

/*******************  FUNCTION  *********************/
void StackPeakTracker::update ( ticks time, ssize_t delta, const PeakTracker& tracker )
{
	//if peak just append the current stack is responsible so update local before
	//copying the peak value.
	//if peak append long time before, copy the old value immediatly
	//This is a trick maybe a better solution exist, but it work like this !
	if (time == tracker.getPeakTime())
	{
		PeakTracker::update(time,delta);
		this->updateOnGlobalPeak(tracker);
	} else {
		this->updateOnGlobalPeak(tracker);
		PeakTracker::update(time,delta);
	}
}

/*******************  FUNCTION  *********************/
void StackPeakTracker::updateOnGlobalPeak ( const PeakTracker& tracker )
{
	if (tracker.getPeakId() > this->lastGlobalPeakId)
	{
		this->lastGlobalPeakId = tracker.getPeakId();
		this->onGlobalPeak = this->current;
	}
}

/*******************  FUNCTION  *********************/
void convertToJson ( htopml::JsonState& json, const StackPeakTracker& value )
{
	json.openStruct();
		json.printField("finalValue",value.current);
		json.printField("localPeakTime",value.peakTime);
		json.printField("localPeak",value.peakValue);
		json.printField("onGlobalPeakTime",value.onGlobalPeak);
	json.closeStruct();
}

}
