/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 02/2015
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MATT_PEAK_TRACKER_HPP
#define MATT_PEAK_TRACKER_HPP

/********************  HEADERS  *********************/
#include <cycle.h>
#include <cstdlib>
#include <json/JsonState.h>

/********************  NAMESPACE  *******************/
namespace MATT
{

/*********************  CLASS  **********************/
class PeakTracker
{
	public:
		PeakTracker(void);
		void update( ticks time, ssize_t delta );
		friend void convertToJson( htopml::JsonState& json, const PeakTracker& value );
		size_t getPeakId(void) const {return peakId;};
		size_t getPeakValue(void) const {return peakValue;};
		size_t getCurrent(void) const {return current;};
		size_t getPeakTime(void) const {return peakTime;};
	protected:
		ticks peakTime;
		size_t peakId;
		size_t peakValue;
		size_t current;
};

/*********************  CLASS  **********************/
class StackPeakTracker : public PeakTracker
{
	public:
		StackPeakTracker(void);	
		void update( ticks time, ssize_t delta ,const PeakTracker & tracker);
		friend void convertToJson( htopml::JsonState& json, const StackPeakTracker& value );
	protected:
		void updateOnGlobalPeak(const PeakTracker & tracker);
	protected:
		size_t lastGlobalPeakId;
		size_t onGlobalPeak;
};

}

#endif //MATT_PEAK_TRACKER_HPP
