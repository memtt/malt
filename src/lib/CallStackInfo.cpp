/********************  HEADERS  *********************/
#include <cstdlib>
#include <json/JsonState.h>
#include "CallStackInfo.h"

/*******************  FUNCTION  *********************/
SimpleQuantityHistory::SimpleQuantityHistory(void )
{
	this->count = 0;
	this->max = 0;
	this->min = 0;
	this->sum = 0;
}

/*******************  FUNCTION  *********************/
void SimpleQuantityHistory::addEvent(ssize_t value)
{
	if (this->count == 0)
	{
		this->min = value;
		this->max = value;
	} else {
		if (value < min)
			min = value;
		if (value > max)
			max = value;
	}
	this->count++;
	this->sum+=value;
}

/*******************  FUNCTION  *********************/
void CallStackInfo::addEvent(ssize_t value)
{
	if (value == 0)
	{
		cntZeros++;
	} else if (value > 0) {
		this->alloc.addEvent(value);
	} else {
		this->free.addEvent(value);
	}
}

/*******************  FUNCTION  *********************/
CallStackInfo::CallStackInfo(void )
{
	this->cntZeros = 0;
}

/*******************  FUNCTION  *********************/
CallStackInfo::~CallStackInfo(void )
{

}

/*******************  FUNCTION  *********************/
void typeToJson(htopml::JsonState& json, std::ostream& stream, const CallStackInfo& value)
{
	json.openStruct();
	json.printField("countZeros",value.cntZeros);
	json.printField("alloc",value.alloc);
	json.printField("free",value.free);
	json.closeStruct();
}

/*******************  FUNCTION  *********************/
std::ostream& operator<<(std::ostream& out, const CallStackInfo& info)
{
	out << "count = " << info.cntZeros;
	return out;
}

/*******************  FUNCTION  *********************/
void typeToJson(htopml::JsonState& json, std::ostream& stream, const SimpleQuantityHistory& value)
{
	json.openStruct();
	json.printField("count",value.count);
	json.printField("min",value.min);
	json.printField("max",value.max);
	json.printField("sum",value.sum);
	json.closeStruct();
}
