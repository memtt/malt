/*****************************************************
             PROJECT  : MALT
             VERSION  : 1.2.2
             DATE     : 10/2020
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MALT_PROFILED_VALUE_HPP
#define MALT_PROFILED_VALUE_HPP

/********************  HEADERS  *********************/
//std
#include <cstdlib>
#include <cassert>
#include <cstring>
//from fftw
#include <cycle.h>
//extern deps
#include <json/JsonState.h>
//internal
#include <common/Debug.hpp>

/********************  MACRO  ***********************/
/**
 * Define how much steps to accumulate in a small buffer before starting to 
 * reduce the values.
**/
#define MALT_PROFILED_STATE_VALUE_FIRST_STEPS 10

/********************  NAMESPACE  *******************/
namespace MALT
{

/*********************  CLASS  **********************/
/**
 * Class to manage time tracking of composed structures and to made reduction on them to maintain
 * a limited number of internal values.
 * It also associate a stack to each values to remember the caller who generate the extremal values.
 * The types used here must provide :
 *   - default constructor to init
 *   - Operator =
 *   - Reduction operator reduce() which return a boolean to true if a new max value has been found internally
 *   - conversion to json
 * 
 * @todo Check for usage of internal allocator instead of default one.
 * @brief Class to manage time tracking of multiple synchone values into composed structure.
**/
template <class T>
class ProfiledValue
{
	public:
		ProfiledValue(int steps = 1024,bool bandwidth = false);
		~ProfiledValue(void);
		void push(ticks t,const T & value,void * location = NULL);
		void flush(void);
		bool isNewPoint(ticks t);
	public:
		template <class U> friend void convertToJson(htopml::JsonState& json, const ProfiledValue<U> & value);
	private:
		void reshape(ticks t);
		void firstShape(void);
		void pushFirst(ticks t,const T & value,void * location);
		void pushNext(ticks t, const T & value,void * location);
		void updateStartEnd(ticks start,ticks end);
		int getLastTouchedId(void) const;
	private:
		//no copy
		ProfiledValue(const ProfiledValue & orig);
		ProfiledValue & operator = (const ProfiledValue & orig);
	private:
		/** It uses a first minimal number of points to determine a good time steps before accumulating. **/
		int cntFirstPoints;
		/** Storage for the first steps before accumulation (data). **/
		T firstPoints[MALT_PROFILED_STATE_VALUE_FIRST_STEPS];
		/** Storage for the first steps before accumulation (times). **/
		ticks firstPointsTicks[MALT_PROFILED_STATE_VALUE_FIRST_STEPS];
		/** Storage for the first steps before accumulation (locations).**/
		void * firstLocations[MALT_PROFILED_STATE_VALUE_FIRST_STEPS];
		/** Storage of time points data corresponding to CELL_ID * perPoints in time. **/
		T * points;
		/** 
		 * Storage of the old time points data corresponding to CELL_ID * perPoints in time. 
		 * It is used as temporar storage by swaping arrays for reshaping when requiring to update
		 * the perPoints variable.
		 **/
		T * oldPoints;
		/** Track the peak value.**/
		T peak;
		/** Remembre if it is touched or not to complete datas before generating json output. **/
		bool * touched;
		/** Remembre if it is touched or not to complete datas before generating json output. **/
		bool * oldTouched;
		/** 
		 * Storage of call location of the last update of the current cell, so the generator of the
		 * largest value if push() use a "larger than" operator.
		**/
		void ** locations;
		/**
		 * Storage of call location of the last update of the current cell, so the generator of the
		 * largest value if push() use a "larger than" operator.
		**/
		void ** oldLocations;
		/** Remember the final timetep to compute the total tracking time. **/
		ticks start;
		/** Remember the final timetep to compute the total tracking time. **/
		ticks end;
		/** Count the number of steps in use. **/
		int steps;
		/** Define the time (ticks) per point to convert ids to time. **/
		ticks perPoints;
		/** Check if already flushed before using json conversion. **/
		bool flushed;
		/** 
		 * Determine with we are measuring bandiwdth or fixed values, so if we need to propagate
		 * previous values or zero values on untouched intervals.
		**/
		bool bandwidth;
};

/*********************  CLASS  **********************/
/**
 * @brief Helper class to profiler simple scalars with max operator in ProfiledValue system.
**/
template <class T>
struct ProfilableMaxScalar
{
	T value;
	ProfilableMaxScalar(void) {}
	ProfilableMaxScalar(const T & value) {this->value = value;}
	bool reduce(const ProfilableMaxScalar<T> & v) {if (v.value > this->value) {this->value = v.value;return true;} else {return false;}}
	template <class U> friend void convertToJson(htopml::JsonState& json, const ProfilableMaxScalar<U> & value) {json.getStream() << value.value;}
	//meta description
	static const char * selfDescribeFields[1];
};

/*********************  CLASS  **********************/
typedef ProfiledValue< ProfilableMaxScalar<double> > ProfiledStateMaxReal;
typedef ProfiledValue< ProfilableMaxScalar<size_t> > ProfiledStateMaxInt;

/**********************  CONST  *********************/
template <class T>
const char * ProfilableMaxScalar<T>::selfDescribeFields[1] = {"max"};

/*******************  FUNCTION  *********************/
template <class T>
void swap(T & v1, T & v2)
{
	T tmp = v1;
	v1 = v2;
	v2 = tmp;
}

/*******************  FUNCTION  *********************/
template <class T>
ProfiledValue<T>::ProfiledValue(int steps,bool bandwidth)
{
	//errors
	assert(steps > 0);
	
	//set defaults
	this->cntFirstPoints = 0;
	this->steps = steps;
	this->start = 0;
	this->end = 0;
	this->perPoints = 0;
	this->flushed = false;
	this->bandwidth = bandwidth;
	
	//allocate memory
	this->points = new T[steps];
	this->oldPoints = new T[steps];
	this->touched = new bool[steps];
	this->oldTouched = new bool[steps];
	this->locations = new void*[steps];
	this->oldLocations = new void*[steps];
	
	//init
	for (int i = 0 ; i < steps ; i++)
	{
		this->touched[i] = false;
		this->oldTouched[i] = false;
		this->locations[i] = NULL;
		this->oldLocations[i] = NULL;
	}
}

/*******************  FUNCTION  *********************/
template <class T>
ProfiledValue<T>::~ProfiledValue(void )
{
	delete [] this->points;
	delete [] this->oldPoints;
	delete [] this->touched;
	delete [] this->oldTouched;
	delete [] this->locations;
	delete [] this->oldLocations;
}

/*******************  FUNCTION  *********************/
template <class T>
void ProfiledValue<T>::push(ticks t, const T& value,void * location)
{
	//update peak
	if (this->cntFirstPoints == 0)
		this->peak = value;
	else
		this->peak.reduce(value);
	
	//if not have all first points, aggregate
	if (this->cntFirstPoints < MALT_PROFILED_STATE_VALUE_FIRST_STEPS)
	{
		this->pushFirst(t,value,location);
	} else if (this->cntFirstPoints == MALT_PROFILED_STATE_VALUE_FIRST_STEPS) {
		this->firstShape();
		this->push(t, value,location);
	} else {
		this->pushNext(t,value,location);
	}
	
	//mark as changed
	this->flushed = false;
}

/*******************  FUNCTION  *********************/
template <class T>
void ProfiledValue<T>::pushFirst(ticks t, const T& value,void * location)
{
	firstPoints[this->cntFirstPoints] = value;
	firstPointsTicks[this->cntFirstPoints] = t;
	firstLocations[this->cntFirstPoints] = location;
	this->cntFirstPoints++;
}

/*******************  FUNCTION  *********************/
template <class T>
void ProfiledValue<T>::pushNext(ticks t, const T& value,void * location)
{
	//check interval
	assumeArg(t >= start,"Invalid t (%1) smaller than start (%2) !").arg(t).arg(start).end();
	
	//check grow
	if (t >= end)
		this->reshape(t);
	
	//compute id
	int id = (t - start) / perPoints;
	
	//check if touched
	if (touched[id])
	{
		bool tmp = points[id].reduce(value);
		if (tmp || locations[id] == NULL)
			locations[id] = location;
	} else {
		points[id] = value;
		touched[id] = true;
		locations[id] = location;
	}
}

/*******************  FUNCTION  *********************/
template <class T>
bool ProfiledValue<T>::isNewPoint(ticks t)
{
	if (this->cntFirstPoints <= MALT_PROFILED_STATE_VALUE_FIRST_STEPS)
		return true;
	if (t >= end)
		return true;
	int id = (t - start) / perPoints;
	if (touched[id] == false)
		return true;
	return false;
}

/*******************  FUNCTION  *********************/
template <class T>
void ProfiledValue<T>::firstShape(void )
{
	//init min/max
	ticks min = firstPointsTicks[0];
	ticks max = firstPointsTicks[0];
	int oldCnt = cntFirstPoints;
	
	//search
	for (int i = 0 ; i < cntFirstPoints ; i++)
	{
		if (firstPointsTicks[0] < min) min = firstPointsTicks[0];
		if (firstPointsTicks[0] > max) max = firstPointsTicks[0];
	}
	
	//compute
	this->updateStartEnd(min,max);
	
	//mark end
	this->cntFirstPoints = MALT_PROFILED_STATE_VALUE_FIRST_STEPS + 1;
	
	//flush values
	for (int i = 0 ; i < oldCnt ; i++)
		this->push(firstPointsTicks[i],firstPoints[i],firstLocations[i]);
}

/*******************  FUNCTION  *********************/
template <class T>
void ProfiledValue<T>::updateStartEnd(ticks start, ticks end)
{
	//compute
	end = start + (end - start) * 2;
	
	//setup per points
	this->perPoints = (end - start) / steps;
	
	//fix min
	if (this->perPoints <= 0)
		this->perPoints = 1;
	
	//round end
	this->start = start;
	this->end = start + (steps * perPoints);
}

/*******************  FUNCTION  *********************/
template <class T>
void ProfiledValue<T>::reshape(ticks t)
{
	//save old start/end
	ticks oldStart = start;
	ticks oldEnd = end;
	ticks oldPerPoints = perPoints;
	
	//update start/end
	updateStartEnd(start,t);
	
	//check same
	if (start == oldStart && end == oldEnd)
		return;
	
	//swap
	swap(points,oldPoints);
	swap(touched,oldTouched);
	swap(locations,oldLocations);
	
	//reset touched
	for (int i = 0 ; i < steps ; i++)
		touched[i] = false;
	
	//refill
	for (int i = 0 ; i < steps ; i++)
	{
		if (oldTouched[i])
		{
			//compute t
			ticks oldT = oldStart + i * oldPerPoints;
			push(oldT,oldPoints[i],oldLocations[i]);
		}
	}
}

/*******************  FUNCTION  *********************/
template <class T>
int ProfiledValue<T>::getLastTouchedId(void ) const
{
	int last = -1;
	
	for (int i = 0 ; i < steps ; i++)
		if (touched[i])
			last = i;
		
	return last;
}

/*******************  FUNCTION  *********************/
template <class T>
void ProfiledValue<T>::flush(void )
{
	//flush first touch
	if (cntFirstPoints <= MALT_PROFILED_STATE_VALUE_FIRST_STEPS)
		firstShape();
	
	//search nb touched
	int cntTouched = getLastTouchedId() + 1;
	
	//propage values
	T zero;
	T * lastPoint = &zero;
	
	//copy untouched values
	for (int i = 0 ; i < cntTouched ; i++)
	{
		if (touched[i] && !bandwidth)
		{
			lastPoint = &points[i];
		} else if (!touched[i] && lastPoint != NULL) {
			points[i] = *lastPoint;
			locations[i] = NULL;
		}
	}
	
	//mark as flushed
	this->flushed = true;
}

/*******************  FUNCTION  *********************/
template <class T>
void convertToJson(htopml::JsonState& json, const ProfiledValue<T>& value)
{
	//errors
	assert(value.flushed);
	
	//locals
	(*(ProfiledValue<T>*)&value).flush();
	int lastId = value.getLastTouchedId();

	//export struct
	json.openStruct();

	//tmp
	size_t fieldCnt = sizeof(T::selfDescribeFields) / sizeof(char*);
	
	//global infos
	json.printField("start",value.start);
	json.printFieldArray("fields",T::selfDescribeFields,fieldCnt);
	json.printField("perPoints",value.perPoints);
	json.printField("peak",value.peak);
	
	//values
	json.printFieldArray("values",value.points,lastId+1);
	json.printFieldArray("callsite",value.locations,lastId+1);

	json.closeStruct();
}

}

#endif //MALT_PROFILED_VALUE_HPP
