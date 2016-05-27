/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 02/2015
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
CallCounter::CallCounter ( void )
{
	this->count = 0;
	this->max = 0;
	this->min = 0;
	this->sum = 0;
}

/*******************  FUNCTION  *********************/
CallCounter& CallCounter::operator+=(const CallCounter& obj)
{
	this->sum += obj.sum;
	this->count += obj.count;
	if (obj.max > this->max)
		this->max = obj.max;
	if (obj.min < this->min)
		this->min = obj.min;
}

/*******************  FUNCTION  *********************/
void CallCounter::call ( ticks t )
{
	this->count++;
	this->sum += t;
	if (this->min == 0 || t < this->min)
		this->min = t;
	if (this->max == 0 || t > this->max)
		this->max = t;
}

/*******************  FUNCTION  *********************/
void convertToJson ( htopml::JsonState& json, const CallCounter& value )
{
	json.openStruct();
		json.printField("count",value.count);
		json.printField("sum",value.sum);
		json.printField("min",value.min);
		json.printField("max",value.max);
	json.closeStruct();
}

/*******************  FUNCTION  *********************/
CallTimeSizeCounter::CallTimeSizeCounter ( void )
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
		json.printField("sumTime",value.sum);
		json.printField("minTime",value.min);
		json.printField("maxTime",value.max);
		json.printField("sumSize",value.sumSize);
		json.printField("minSize",value.minSize);
		json.printField("maxSize",value.maxSize);
	json.closeStruct();
}

}