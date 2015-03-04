/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 02/2015
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
#include <cassert>
#include <sys/time.h>
#include <unistd.h>
#include <cstdio>
#include "Clock.hpp"

/********************  NAMESPACE  *******************/
namespace MATT
{

/*******************  FUNCTION  *********************/
Clock::Clock()
{
	this->eventId = 0;
	this->lastEvent = 0;
	this->startCycles = getticks();
	gettimeofday(&startTimeval,NULL);
}

/*******************  FUNCTION  *********************/
Clock::Clock(const Clock & clock)
{
	this->eventId = clock.eventId;
	this->lastEvent = clock.lastEvent;
	this->startCycles = clock.startCycles;
}

/*******************  FUNCTION  *********************/
ticks Clock::get ( ClockMode mode ) const
{
	assert(mode == CLOCK_TICKS || mode == CLOCK_ID);
	if (mode == CLOCK_TICKS)
		return getticks() - this->startCycles;
	else
		return eventId;
}

/*******************  FUNCTION  *********************/
ticks Clock::getLastEventTime ( ClockMode mode ) const
{
	assert(mode == CLOCK_TICKS || mode == CLOCK_ID);
	if (mode == CLOCK_TICKS)
		return lastEvent;
	else
		return eventId;
}

/*******************  FUNCTION  *********************/
void Clock::markEvent ( void )
{
	eventId++;
	lastEvent = getticks() - this->startCycles;
}

/*******************  FUNCTION  *********************/
ticks Clock::getStart ( void )
{
	return startCycles;
}

/*******************  FUNCTION  *********************/
ticks Clock::getTicksPerSecond ( void ) const
{
	timeval tSec;
	ticks tTicks;
	ticks res;
	
	//read
	tTicks = getticks();
	gettimeofday(&tSec,NULL);
	
	//compute delta and store
	timeval delta;
	timersub(&tSec,&startTimeval,&delta);
	
	//if too chost, sleep a little and return
	if (delta.tv_sec == 0 && delta.tv_usec < 200000)
	{
		fprintf(stderr,"MATT : Using usleep to get better ticks <-> seconds conversion !\n");
		usleep(200000);
		res = this->getTicksPerSecond();
	} else {
		res = (double)(tTicks-startCycles)/((double)delta.tv_sec + (double)delta.tv_usec/(double)1000000.0);
	}
	
	return res;
}

};
