/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 02/2015
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
#include <common/Helpers.hpp>
#include <allocators/NoFreeAllocator.hpp>
#include <common/Debug.hpp>
#include "TimeProfiler.hpp"

/********************  NAMESPACE  *******************/
namespace MATT
{

/*******************  FUNCTION  *********************/
TimeProfiler::TimeProfiler (int entries, int size , bool lastOnNotTouched)
{
	//defaults
	this->steps = 1;
	this->size = size;
	this->entries = entries;
	this->lastOnNotTouched = lastOnNotTouched;
	
	//names
	this->entryNames = new (noFreeMalloc<std::string>(entries))std::string[entries];
	
	//values
	this->values = noFreeMalloc<ValueType>(size * entries);
	this->oldValues = noFreeMalloc<ValueType>(size * entries);
	
	//touched
	this->touched = noFreeMalloc<bool>(size * entries);
	this->oldTouched = noFreeMalloc<bool>(size * entries);
	
	//stacks
	this->stackIds = noFreeMalloc<StackId>(size );
	this->oldStackIds = noFreeMalloc<StackId>(size);
	
	//setup
	this->reset();
}

/*******************  FUNCTION  *********************/
int TimeProfiler::getMaxTouchedId ( void ) const
{
	int ret = 0;
	for (int i = 0 ; i < this->size ; i++)
		for (int j = 0 ; j < entries ; j++)
			if (this->touched[j*size+i])
				ret = i;
	return ret;
}

/*******************  FUNCTION  *********************/
void TimeProfiler::reset ( void )
{
	for (int i = 0 ; i < this->size ; i++)
	{
		this->stackIds[i] = -1;
		for (int j = 0 ; j < this->entries ; j++)
		{
			this->touched[j*size + i] = false;
			this->values[j * size + i] = 0;
		}
	}
}

/*******************  FUNCTION  *********************/
void TimeProfiler::reshape ( ticks time )
{
	//update steps
	while (size*steps <= time)
		steps *= 2;
	
	//permut
	Helpers::permut(this->values,this->oldValues);
	Helpers::permut(this->touched,this->oldTouched);
	Helpers::permut(this->stackIds,this->oldStackIds);
	
	//reset
	this->reset();
	
	//fill
	for (size_t i = 0 ; i < this->size ; i++)
		for (int j = 0 ; j < entries ; j++)
			if (oldTouched[j * size + i])
				this->push(i,oldStackIds[i],j,oldValues[j * size + i]);
}

/*******************  FUNCTION  *********************/
void TimeProfiler::push ( ticks time, StackId stackId,int entryId, const ValueType & value )
{
	assumeArg(entryId < entries,"Invalid entry ID : %1, must be lower than %2 !").arg(entryId).arg(entries).end();
	
	//need reshape
	if (time >= steps * size)
		this->reshape(time);
	
	//get ids
	size_t id = time / steps;
	
	//update
	ValueType & v = this->values[id+(size*entryId)];
	if (value > v || this->touched[id+(size*entryId)] == false)
	{
		v = value;
		this->stackIds[id] = stackId;
	}
	
	//touch
	this->touched[id+(size*entryId)] = true;
}

/*******************  FUNCTION  *********************/
void convertToJson ( htopml::JsonState& json, const TimeProfiler & value )
{
	size_t maxId = value.getMaxTouchedId();
	bool lastOnNotTouched = value.lastOnNotTouched;
	
	//check names
	for (int i = 0 ; i < value.entries ; i++)
		assumeArg(value.entryNames[i].empty() == false,"Missing entryName in TimeProfiler object for ID %1 !").arg(i).end();
	
	json.openStruct();
		json.printField("steps",value.steps);
		json.printFieldArray("stackIds",value.stackIds,maxId);
		for (int j = 0 ; j < value.entries ; j++)
		{
			json.openFieldArray(value.entryNames[j].c_str());
				TimeProfiler::ValueType last = 0;
				for (int i = 0 ; i < maxId ; i++)
				{
					if (lastOnNotTouched == false || value.touched[j*value.size+i])
					{
						last = value.values[j*value.size+i];
						json.printValue(last);
					} else {
						json.printValue(last);
					}
				}
			json.closeFieldArray(value.entryNames[j].c_str());
		}
	json.closeStruct();
}

/*******************  FUNCTION  *********************/
void TimeProfiler::registerEntry ( int id, const char* name )
{
	assumeArg(id < entries,"Invalid entry ID : %1, must be lower than %2 !").arg(id).arg(entries).end();
	entryNames[id]=name;
}

};
