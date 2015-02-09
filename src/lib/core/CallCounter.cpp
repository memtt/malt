/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
#include "CallCounter.hpp"
#include <json/JsonState.h>

/********************  NAMESPACE  *******************/
namespace MATT
{

/*******************  FUNCTION  *********************/
CallCounter::CallCounter ( bool time )
{
	this->time = time;
	this->count = 0;
	this->maxTime = 0;
	this->minTime = 0;
	this->sumTime = 0;
}

/*******************  FUNCTION  *********************/
void CallCounter::call ( ticks t )
{
	this->count++;
	this->sumTime += t;
	if (this->minTime == 0 || t < this->minTime)
		this->minTime = t;
	if (this->maxTime == 0 || t > this->maxTime)
		this->maxTime = t;
}

/*******************  FUNCTION  *********************/
void convertToJson ( htopml::JsonState& json, const CallCounter& value )
{
	json.openStruct();
	if (value.time)
	{
		json.printField("count",value.count);
		json.printField("sumTime",value.sumTime);
		json.printField("minTime",value.minTime);
		json.printField("maxTime",value.maxTime);
	} else {
		json.printField("count",value.count);
		json.printField("sum",value.sumTime);
		json.printField("min",value.minTime);
		json.printField("max",value.maxTime);
	}
	json.closeStruct();
}

/*******************  FUNCTION  *********************/
CallTimeSizeCounter::CallTimeSizeCounter ( void )
	:CallCounter(true)
{

}

/*******************  FUNCTION  *********************/
void CallTimeSizeCounter::call ( ticks t, size_t s )
{
	CallCounter::call(t);
	this->sumSize += s;
	if (this->minSize == 0 || s < this->minSize)
		this->minSize = s;
	if (this->maxSize == 0 || s > this->maxSize)
		this->maxSize = s;
}

/*******************  FUNCTION  *********************/
void convertToJson ( htopml::JsonState& json, const CallTimeSizeCounter& value )
{
	json.openStruct();
		json.printField("count",value.count);
		json.printField("sumTime",value.sumTime);
		json.printField("minTime",value.minTime);
		json.printField("maxTime",value.maxTime);
		json.printField("sumSize",value.sumTime);
		json.printField("minSize",value.minTime);
		json.printField("maxSize",value.maxTime);
	json.closeStruct();
}

}