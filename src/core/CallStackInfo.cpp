/********************  HEADERS  *********************/
#include <cstdlib>
#include <json/JsonState.h>
#include "CallStackInfo.hpp"

/*******************  NAMESPACE  ********************/
namespace ATT
{

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
void SimpleQuantityHistory::push(SimpleQuantityHistory& value)
{
	this->count += value.count;
	this->max += value.max;
	this->min += value.min;
	this->sum += value.sum;
}

/*******************  FUNCTION  *********************/
void CallStackInfo::addEvent(ssize_t value, ticks lifetime)
{
	if (value == 0)
	{
		cntZeros++;
	} else if (value > 0) {
		this->alloc.addEvent(value);
	} else {
		this->free.addEvent(-value);
	}
	
	if (value < 0 && lifetime != 0)
		this->lifetime.addEvent(lifetime);
	
	//update alive memory
	this->alive+=value;
	if (this->alive > this->maxAlive)
		this->maxAlive = this->alive;
}

/*******************  FUNCTION  *********************/
void CallStackInfo::onFreeLinkedMemory(ssize_t value, ticks lifetime)
{
	assert(alive <= 0);
	this->alive += value;
	if (lifetime != 0)
		this->lifetime.addEvent(lifetime);
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
void CallStackInfo::push(CallStackInfo& info)
{
	this->alive += info.alive;
	this->maxAlive += info.maxAlive;
	this->cntZeros += info.cntZeros;
	this->alloc.push(info.alloc);
	this->free.push(info.free);
	this->lifetime.push(info.lifetime);
}

/*******************  FUNCTION  *********************/
void typeToJson(htopml::JsonState& json, std::ostream& stream, const CallStackInfo& value)
{
	json.openStruct();
	json.printField("countZeros",value.cntZeros);
	json.printField("maxAlive",value.maxAlive);
	json.printField("alloc",value.alloc);
	json.printField("free",value.free);
	json.printField("lifetime",value.lifetime);
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

/*******************  FUNCTION  *********************/
void CallStackInfo::writeAsCallgrindEntry(int line, std::ostream& out) const
{
	out << line << " " << maxAlive << " " << alloc.sum << " "<< alloc.count;
}

}
