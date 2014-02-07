/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
//standard
#include <cstdlib>
//htopml
#include <json/JsonState.h>
//internals
#include "CallStackInfo.hpp"
#include <common/Debug.hpp>

/*******************  NAMESPACE  ********************/
namespace MATT
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
	this->sum += value.sum;

	if (this->max > value.max)
		this->max = value.max;
	if (this->min < value.min)
		this->min += value.min;
}

/*******************  FUNCTION  *********************/
void CallStackInfo::onFreeEvent(size_t value, ticks lifetime)
{
	if (value == 0)
	{
		cntZeros++;
	} else {
		this->free.addEvent(value);
	}
	
	if (lifetime != 0)
		this->lifetime.addEvent(lifetime);
	
	//update alive memory
	//this->alive-=value;
	assert(this->alive >= 0);
	if (this->alive > this->maxAlive)
		this->maxAlive = this->alive;
}

/*******************  FUNCTION  *********************/
void CallStackInfo::onAllocEvent(size_t value)
{
	//update alloc counters
	if (value == 0)
		cntZeros++;
	else
		this->alloc.addEvent(value);
	
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
	this->alive = 0;
	this->maxAlive = 0;
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
void convertToJson(htopml::JsonState& json, const CallStackInfo& value)
{
	json.openStruct();
	json.printField("countZeros",value.cntZeros);
	json.printField("maxAliveReq",value.maxAlive);
	json.printField("aliveReq",value.alive);
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
void convertToJson(htopml::JsonState& json, const SimpleQuantityHistory& value)
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
// 	assert(alive < 0);
// 	if (alive < 0)
// 		warning("Get negativ alive value, set to 0 !");
// 	out << line << ' ' << maxAlive << ' ' << alive
// 	    << ' ' << alloc.count << ' ' << alloc.min << ' ' << alloc.max << ' ' << alloc.sum 
// 	    << ' ' << free.sum << ' ' << free.min << ' ' << free.max << ' ' << free.sum
// 	    << ' ' << lifetime.min << ' ' << lifetime.max
// 	    << ' ' << alloc.count + free.count + cntZeros;
	out << line << ' ' << alloc.count << ' ' << free.count << ' ' <<  alloc.count + free.count + cntZeros << ' ' << alloc.sum;
}

/*******************  FUNCTION  *********************/
void CallStackInfo::writeCallgrindEventDef(std::ostream& out)
{
// 	out << "events: MaxAliveMemory AliveMemory "
// 	    << "AllocCnt AllocMin AllocMax AllocSum "
// 		<< "FreeSum FreeMin FreeMax FreeSum "
// 		<< "LifetimeMin LifetimeMax "
// 		<< "MemOps\n";
	out << "events: AllocCnt FreeCnt MemOps AllocSum\n";
}

}
