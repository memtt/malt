/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 02/2015
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MATT_CLOCK_HPP
#define MATT_CLOCK_HPP

/********************  HEADERS  *********************/
#include <cycle.h>

/********************  NAMESPACE  *******************/
namespace MATT
{

/********************  ENUM  ************************/
enum ClockMode
{
	CLOCK_TICKS,
	CLOCK_ID
};

/*********************  CLASS  **********************/
class Clock
{
	public:
		Clock();
		void markEvent(void);
		ticks get(ClockMode mode) const;
		ticks getLastEventTime(ClockMode mode) const;
		ticks getStart(void);
	protected:
		ticks lastEvent;
		ticks eventId;
		ticks start;
};

}

#endif //MATT_CLOCK_HPP
