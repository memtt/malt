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
	
	assert(max >= min);
}

/*******************  FUNCTION  *********************/
void SimpleQuantityHistory::push(const SimpleQuantityHistory& value)
{
	if (this->count == 0)
	{
		this->min = value.min;
		this->max = value.max;
	} else {
		if (value.max > this->max)
			this->max = value.max;
		if (value.min < this->min)
			this->min = value.min;
	}
	
	this->count += value.count;
	this->sum += value.sum;
	
	assert(max >= min);
}

/*******************  FUNCTION  *********************/
void CallStackInfo::onFreeEvent(size_t value,size_t peakId)
{
	if (value == 0)
	{
		cntZeros++;
	} else {
		updatePeak(peakId);
		this->free.addEvent(value);
	}
}

/*******************  FUNCTION  *********************/
ssize_t SimpleQuantityHistory::getMean(void) const
{
	if (count == 0)
		return 0;
	else
		return sum/count;
}

/*******************  FUNCTION  *********************/
void CallStackInfo::updatePeak(size_t peakId)
{
	if (this->peakId < peakId)
	{
		this->peakId = peakId;
		this->peak = this->alive;
	}
}

/*******************  FUNCTION  *********************/
void CallStackInfo::onAllocEvent(size_t value,size_t peakId)
{
	//update alloc counters
	if (value == 0)
		cntZeros++;
	else
		this->alloc.addEvent(value);
	
	//update peak
	updatePeak(peakId);
	
	//update alive memory
	this->alive+=value;
	if (this->alive > this->maxAlive)
		this->maxAlive = this->alive;
}

/*******************  FUNCTION  *********************/
void CallStackInfo::onFreeLinkedMemory(size_t value, ticks lifetime,size_t peakId)
{
	assert(alive >= value);
	assert(alive >= 0);
	
	updatePeak(peakId);

	this->alive -= value;
	if (lifetime != 0)
		this->lifetime.addEvent(lifetime);
}

/*******************  FUNCTION  *********************/
CallStackInfo::CallStackInfo(void )
{
	this->alive = 0;
	this->maxAlive = 0;
	this->cntZeros = 0;
	this->peak = 0;
	this->peakId = 0;
}

/*******************  FUNCTION  *********************/
CallStackInfo::~CallStackInfo(void )
{
}

/*******************  FUNCTION  *********************/
void CallStackInfo::push(const CallStackInfo& info)
{
	this->alive += info.alive;
	this->maxAlive += info.maxAlive;
	this->cntZeros += info.cntZeros;
	this->alloc.push(info.alloc);
	this->free.push(info.free);
	this->lifetime.push(info.lifetime);
	this->peak += info.peak;
	//assert(peakId == info.peakId);
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
	json.printField("globalPeak",value.peak);
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
	assert(alloc.max >= alloc.min);
	assert(free.max >= free.min);
	assert(alive >= 0);
	assert(maxAlive >= 0);
	
	out << line << ' ' << alloc.count << ' ' << free.count << ' ' <<  alloc.count + free.count + cntZeros 
		<< ' ' << alloc.sum << ' ' << free.sum
		<< ' ' << alive << ' ' << maxAlive;
}

/*******************  FUNCTION  *********************/
void CallStackInfo::writeCallgrindEventDef(std::ostream& out)
{
	out << "events: AllocCnt FreeCnt MemOps"
		<< " AllocSum FreeSum"
		<< " Leaks AliveReqMax\n";
}

}
