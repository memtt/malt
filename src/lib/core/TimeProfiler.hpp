/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 02/2015
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MATT_TIME_PROFILER_HPP
#define MATT_TIME_PROFILER_HPP

/********************  HEADERS  *********************/
#include <cycle.h>
#include <stacks/StackTree.hpp>

/********************  NAMESPACE  *******************/
namespace MATT
{

#define MATT_TIME_PROFILER_DEFAULT_SIZE 512

/*********************  CLASS  **********************/
class TimeProfiler
{
	public:
		typedef ssize_t ValueType;
	public:
		TimeProfiler(int entries,int size = 512,bool bandwidth = false);
		void registerEntry(int id,const char * name);
		void push(ticks time,StackId stackId,int id,const ValueType & value);
		friend void convertToJson( htopml::JsonState& json, const TimeProfiler & value );
		bool isNewPoint(ticks time, int id) const;
		void offset(int id, ssize_t offset);
	protected:
		void reshape(ticks time);
		void reset(void);
		void allocate(void);
		int getMaxTouchedId(void) const;
	protected:
		bool bandwidth;
		int size;
		int entries;
		ticks steps;
		std::string * entryNames;
		ValueType * values;
		ValueType * oldValues;
		bool * touched;
		bool * oldTouched;
		StackId * stackIds;
		StackId * oldStackIds;
};




}

#endif //MATT_TIME_PROFILER_HPP
