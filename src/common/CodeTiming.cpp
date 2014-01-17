/********************  HEADERS  *********************/
//extern
#include <cassert>
#include <iomanip>
#include <iostream>
//locals
#include "CodeTiming.h"

/***************** USING NAMESPACE ******************/
using namespace std;

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
