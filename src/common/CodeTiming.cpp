#include <iostream>
#include <iomanip>
#include "CodeTiming.h"

using namespace std;

static const char * cstUnits[] = {"","K","M","G","T","P"};

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
	cerr << " , " << "radio : ~ " << std::setprecision( 2 ) << ratio << "% )" << endl;
}

void CodeTiming::start(void)
{
	this->lastStart = getticks();
}

void CodeTiming::end(void)
{
	//get time
	ticks t =getticks() - lastStart;
	
	//update min/max
	if (count == 0)
	{
		min = max = t;
	} else {
		if (t < min) min = t;
		if (t > max) max = t;
	}
	
	//update global sums
	count++;
	sum += t;
}

void CodeTiming::printValue(double value, const char* unit)
{
	int depth = 0;
	while (value > 100)
	{
		depth++;
		value /= 1000.0;
	}
	cerr << value << " " << cstUnits[depth] << unit;
}
