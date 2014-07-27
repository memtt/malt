/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MATT_PROFILED_VALUE_HPP
#define MATT_PROFILED_VALUE_HPP

/********************  HEADERS  *********************/
#include <cstdlib>
#include <cycle.h>
#include <json/JsonState.h>
#include <cassert>
#include <cstring>
#include <common/Debug.hpp>

#define MATT_PROFILED_STATE_VALUE_FIRST_STEPS 10

namespace MATT
{

/*********************  CLASS  **********************/
template <class T>
class ProfiledValue
{
	public:
		ProfiledValue(int steps = 1024);
		~ProfiledValue(void);
		void push(ticks t,const T & value,void * location = NULL);
		void flush(void);
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
		int cntFirstPoints;
		T firstPoints[MATT_PROFILED_STATE_VALUE_FIRST_STEPS];
		ticks firstPointsTicks[MATT_PROFILED_STATE_VALUE_FIRST_STEPS];
		void * firstLocations[MATT_PROFILED_STATE_VALUE_FIRST_STEPS];
		T * points;
		T * oldPoints;
		bool * touched;
		bool * oldTouched;
		void ** locations;
		void ** oldLocations;
		ticks start;
		ticks end;
		int steps;
		ticks perPoints;
		bool flushed;
};

/*********************  CLASS  **********************/
template <class T>
struct ProfilableMaxScalar
{
	T value;
	ProfilableMaxScalar(void) {};
	ProfilableMaxScalar(const T & value) {this->value = value;};
	void set(const ProfilableMaxScalar<T> & v) {this->value = v.value;};
	bool push(const ProfilableMaxScalar<T> & v) {if (v.value > this->value) {this->value = v.value;return true;} else {return false;}};
	template <class U> friend void convertToJson(htopml::JsonState& json, const ProfilableMaxScalar<U> & value) {json.getStream() << value.value;};
};

/*********************  CLASS  **********************/
typedef ProfiledValue< ProfilableMaxScalar<double> > ProfiledStateMaxReal;
typedef ProfiledValue< ProfilableMaxScalar<size_t> > ProfiledStateMaxInt;

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
ProfiledValue<T>::ProfiledValue(int steps)
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
	//if not have all first points, aggregate
	if (this->cntFirstPoints < MATT_PROFILED_STATE_VALUE_FIRST_STEPS)
	{
		this->pushFirst(t,value,location);
	} else if (this->cntFirstPoints == MATT_PROFILED_STATE_VALUE_FIRST_STEPS) {
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
	firstPoints[this->cntFirstPoints].set(value);
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
		bool tmp = points[id].push(value);
		if (tmp || locations[id] == NULL)
			locations[id] = location;
	} else {
		points[id].set(value);
		touched[id] = true;
		locations[id] = location;
	}
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
	this->cntFirstPoints = MATT_PROFILED_STATE_VALUE_FIRST_STEPS + 1;
	
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
	int oldStart = start;
	int oldEnd = end;
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
	if (cntFirstPoints <= MATT_PROFILED_STATE_VALUE_FIRST_STEPS)
		firstShape();
	
	//search nb touched
	int cntTouched = getLastTouchedId() + 1;
	
	//propage values
	T * lastPoint = NULL;
	
	//copy untouched values
	for (int i = 0 ; i < cntTouched ; i++)
	{
		if (touched[i])
		{
			lastPoint = &points[i];
		} else if (lastPoint != NULL) {
			points[i].set(*lastPoint);
			locations[i] = NULL;
		}
	}
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
	
	//global infos
	json.printField("start",value.start);
	json.printField("perPoints",value.perPoints);
	
	//values
	json.printFieldArray("values",value.points,lastId+1);
	json.printFieldArray("callsite",value.locations,lastId+1);

	json.closeStruct();
}

}

#endif //MATT_PROFILED_VALUE_HPP
