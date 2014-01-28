/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
//extern
#include <cassert>
#include <iomanip>
#include <iostream>
//os specific (TODO move)
#include <sys/time.h>
#include <unistd.h>
//locals
#include "CodeTiming.hpp"

/***************** USING NAMESPACE ******************/
using namespace std;

/*******************  NAMESPACE  ********************/
namespace MATT
{

/********************** CONSTS **********************/
/** Strings for unit printing. **/
static const char * cstUnits[] = {"","K","M","G","T","P"};

/*******************  FUNCTION  *********************/
CodeTiming::CodeTiming(const char* name)
{
	this->name = name;
	this->count = 0;
	this->max = 0;
	this->min = 0;
	this->sum = 0;
	this->lastStart = 0;
	this->globalStart = getticks();
}

/*******************  FUNCTION  *********************/
/**
 * The destructor is in charge of printing the final results.
**/
CodeTiming::~CodeTiming(void)
{
	ticks wholeExec = getticks() - globalStart;
	double ratio = (double)(100*sum) / (double)wholeExec;
	cerr << "TIMING OF " << std::setw(32) << std::left << name << " => [ ";
	printValue(min);
	cerr << " , ";
	printValue((double)sum / (double)count);
	cerr << " , ";
	printValue(max);
	cerr << " ] => TOTAL ( calls : ";
	printValue(count);
	cerr << " , time : ";
	printValue(sum);
	cerr << " , " << "radio : ~ " << std::setprecision( 2 ) << setw(8) << std::left << ratio << "% )" << endl;
}

/*******************  FUNCTION  *********************/
/**
 * Help to format values by using adapted unit.
 * @param value The value to print.
 * @param unit The name of the unit to use (cycles, seconds...). Empty for none.
**/
void CodeTiming::printValue(double value, const char* unit)
{
	//errors
	assert(unit != NULL);

	//compte depth
	int depth = 0;
	while (value > 100)
	{
		depth++;
		value /= 1000.0;
	}
	
	//print
	cerr << setw(5) << setprecision(2) << std::right << value << " " << setw(1) << cstUnits[depth] << unit;
}

/*******************  FUNCTION  *********************/
/**
 * @return Return the number of ticks per seconds on the current CPU.
 * The measurement is done by mixing informations from getticks and gettimeofday.
**/
ticks ticksPerSecond(void)
{
	//remember the value to not measure it every time
	static ticks alreadyMeasured = 0;
	
	//measure if required
	if (alreadyMeasured == 0)
	{
		//measure
		timeval ts0,ts1;
		ticks t0,t1;
		gettimeofday(&ts0,NULL);
		t0 = getticks();
		sleep(1);
		t1 = getticks();
		gettimeofday(&ts1,NULL);
		
		//compute delta and store
		timeval delta;
		timersub(&ts1,&ts0,&delta);
		alreadyMeasured = (double)(t1-t0)/((double)delta.tv_sec + (double)delta.tv_usec/(double)1000000.0);
	}
	
	//final return
	return alreadyMeasured;
}

};
