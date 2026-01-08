/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 09/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/core/CallStackInfo.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (ECR) - 2014
*    AUTHOR   : Sébastien Valat - 2014 - 2022
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
//standard
#include <cstdlib>
//htopml
#include <json/JsonState.h>
//internals
#include "CallStackInfo.hpp"
#include <common/Debug.hpp>

/**********************************************************/
namespace MALT
{

/**********************************************************/
#define LINE_BREAK "\n"

/**********************************************************/
/**
 * Constructor the count/min/max/sum storage, mainly to setup values to zero.
**/
SimpleQuantityHistory::SimpleQuantityHistory(void )
{
	this->count = 0;
	this->max = 0;
	this->min = 0;
	this->sum = 0;
}

/**********************************************************/
/**
 * Register a new event and update the count/min/max/sum depening on the given value.
**/
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

/**********************************************************/
/**
 * Made a reduction on the current storage by merging the values from the given one.
**/
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

/**********************************************************/
/**
 * Register a free event on the call stack info object.
 * @param value Define the size of the chunk we deallocate.
 * @param peakId Define the ID of the last global peak seen by the caller. If larger
 * than the local one, update the local peak with current status.
**/
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

/**********************************************************/
/**
 * Register a free event on the call stack info object.
 * @param value Define the size of the chunk we deallocate.
 * @param peakId Define the ID of the last global peak seen by the caller. If larger
 * than the local one, update the local peak with current status.
**/
void CallStackInfo::onGpuFreeEvent(size_t value,size_t peakId)
{
	if (value == 0)
	{
		cntZeros++;
	} else {
		updatePeak(peakId);
		this->gpuFree.addEvent(value);
	}
}

/**********************************************************/
/**
 * @return Compute and return the mean size.
**/
ssize_t SimpleQuantityHistory::getMean(void) const
{
	if (count == 0)
		return 0;
	else
		return sum/count;
}

/**********************************************************/
/**
 * Take care of the peak update.
 * @param peakId Define the ID of the last global peak seen by the caller. If larger
 * than the local one, update the local peak with current status.
**/
void CallStackInfo::updatePeak(size_t peakId)
{
	if (this->peakId < peakId)
	{
		this->peakId = peakId;
		this->peak = this->alive;
	}
}

/**********************************************************/
/**
 * Return the memory consumption at peak time.
**/
size_t CallStackInfo::getPeak(void) const
{
	return this->peak;
}

/**********************************************************/
/**
 * Register a free event on the call stack info object.
 * @param value Define the size of the chunk we allocate.
 * @param peakId Define the ID of the last global peak seen by the caller. If larger
 * than the local one, update the local peak with current status.
**/
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

/**********************************************************/
/**
 * Register a free event on the call stack info object.
 * @param value Define the size of the chunk we allocate.
 * @param peakId Define the ID of the last global peak seen by the caller. If larger
 * than the local one, update the local peak with current status.
**/
void CallStackInfo::onGpuAllocEvent(size_t value,size_t peakId)
{
	//update alloc counters
	if (value == 0)
		cntZeros++;
	else
		this->gpuAlloc.addEvent(value);
	
	//update peak
	updatePeak(peakId);
	
	//update alive memory
	this->alive+=value;
	if (this->alive > this->maxAlive)
		this->maxAlive = this->alive;
}

/**********************************************************/
void CallStackInfo::onMmap ( ssize_t value,size_t peakId )
{
	this->mmap.addEvent(value);
	this->onAllocEvent(value, peakId);
}

/**********************************************************/
void CallStackInfo::onMunmap ( ssize_t value,size_t peakId, bool subMunmap )
{
	this->munmap.addEvent(value);
	//if sub munmap, we should not count
	if (subMunmap)
		this->munmap.count--;
	this->onFreeEvent(value, peakId);
}

/**********************************************************/
/**
 * Free memory which is known to be seen before by the tool. We made the distinction due to some
 * issues with negativ values which might appear on alive parameter otherwise if we missed some allocations.
 * @param value Define the size of the chunk we allocate.
 * @param filetime Define the lifetime of the chunk.
 * @param peakId Define the ID of the last global peak seen by the caller. If larger
 * than the local one, update the local peak with current status.
**/
void CallStackInfo::onFreeLinkedMemory(size_t value, ticks lifetime,size_t peakId)
{
	assert(alive >= (ssize_t)value);
	assert(alive >= 0);
	
	updatePeak(peakId);

	this->alive -= value;
	if (lifetime != 0)
		this->lifetime.addEvent(lifetime);
}

/**********************************************************/
/**
 * Constructor of the call stack info to setup the default values to zero.
**/
CallStackInfo::CallStackInfo(void )
{
	this->reallocCount = 0;
	this->reallocDelta = 0;
	this->alive = 0;
	this->maxAlive = 0;
	this->cntZeros = 0;
	this->peak = 0;
	this->peakId = 0;
}

/**********************************************************/
/**
 * Register a realloc event.
 * @param oldSize define the old size of the chunk.
 * @param newSize define the new size of the chunk (parameter requested by realloc).
**/
void CallStackInfo::onReallocEvent(size_t oldSize, size_t newSize)
{
	this->reallocCount++;
	if (oldSize > newSize)
		this->reallocDelta += oldSize - newSize;
	else
		this->reallocDelta += newSize - oldSize;
}

/**********************************************************/
/**
 * Merge two call stack info on the current one.
 * @param info The remove info to merge on the current one.
**/
void CallStackInfo::merge(const CallStackInfo& info)
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

/**********************************************************/
/**
 * Manage json conversion of call stack info to be compatible with toJson().
**/
void convertToJson(htopml::JsonState& json, const CallStackInfo& value)
{
	json.openStruct();
	json.printField("countZeros",value.cntZeros);
	json.printField("maxAliveReq",value.maxAlive);
	json.printField("aliveReq",value.alive);
	json.printField("alloc",value.alloc);
	json.printField("free",value.free);
	json.printField("gpuAlloc",value.gpuAlloc);
	json.printField("gpuFree",value.gpuFree);
	json.printField("mmap",value.mmap);
	json.printField("munmap",value.munmap);
	json.printField("lifetime",value.lifetime);
	json.printField("globalPeak",value.peak);
	json.printField("reallocCount",value.reallocCount);
	json.printField("reallocSumDelta",value.reallocDelta);
	//json.printField("mmap",value.mmap);
	json.closeStruct();
}

/**********************************************************/
/**
 * Provide ostream compatbility to help debugging in unit tests.
**/
std::ostream& operator<<(std::ostream& out, const CallStackInfo& info)
{
	out << "count = " << info.cntZeros;
	return out;
}

/**********************************************************/
/**
 * Convert the quantiry history into json stream.
 * 
 * @param json The stream to pipe in.
 * @param value The quantiry to dump.
 */
void convertToJson(htopml::JsonState& json, const SimpleQuantityHistory& value)
{
	json.openStruct();
	json.printField("count",value.count);
	json.printField("min",value.min);
	json.printField("max",value.max);
	json.printField("sum",value.sum);
	json.closeStruct();
}

/**********************************************************/
/**
 * Dump the call stack info into kcachegrind compatible format.
**/
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

/**********************************************************/
/**
 * @return The alloc infos.
 */
const SimpleQuantityHistory & CallStackInfo::getAllocInfo ( void ) const
{
	return this->alloc;
}

/**********************************************************/
/**
 * @return The free infos
 */
const SimpleQuantityHistory & CallStackInfo::getFreeInfo ( void ) const
{
	return this->free;
}

/**********************************************************/
/**
 * @return the lifetime infos.
 */
const SimpleQuantityHistory & CallStackInfo::getLifetime ( void ) const
{
	return this->lifetime;
}

/**********************************************************/
/**
 * Check if has realloc.
 * @return True if has seen some realloc, false otherwise.
 */
bool CallStackInfo::hasRealloc ( void ) const
{
	return reallocCount > 0;
}

/**********************************************************/
/**
 * Dump as callgrind call entry.
**/
void CallStackInfo::writeAsCallgrindCallEntry ( int line, std::ostream& out ) const
{
	if (line == -1)
		line = 0;
	out << "calls=" << free.count + alloc.count + cntZeros << " 0" << LINE_BREAK;
}

/**********************************************************/
/**
 * Dump the definition of the internal metrics exported by the call stack info 
 * into the kcachegrind compabitle output format.
**/
void CallStackInfo::writeCallgrindEventDef(std::ostream& out)
{
	out << "events: AllocCnt FreeCnt MemOps"
		<< " AllocSum FreeSum"
		<< " Leaks AliveReqMax\n";
}

}
