/*****************************************************
             PROJECT  : MALT
             VERSION  : 1.2.2
             DATE     : 10/2020
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#include <cycle.h>
#include <stacks/BacktraceStack.hpp>
#include <profiler/AllocStackProfiler.hpp>
#include <stack-tree/RLockFreeTree.hpp>
#include <cstdlib>
#include <ostream>
#include <string>
#include <sys/time.h>
#include <unistd.h>
#include <cassert>
#include <iostream>
#include <cstdio>
#include <iomanip>

using namespace std;
using namespace MALT;

#define MAX_STACKS 10000
#define MAX_LOCAL_REUSE 5
#define MAX_STACK_SIZE 1024

class BenchTiming
{
	public:
		BenchTiming(void);
		void start(const char * name,int duration);
		void eventStart(void);
		void eventEnd(void);
		void end(ostream & out);
		bool needRun(void) const;
	private:
		ticks getTicksPerSecond(void) const;
		void printTime(ostream & out,ticks value);
	private:
		string name;
		ticks perSeconds;
		ticks startTime;
		ticks lastEventStart;
		size_t cntEvents;
		ticks minEventTime;
		ticks maxEventTime;
		ticks expectedEnd;
		ticks sumEvents;
};

BenchTiming::BenchTiming(void)
{
	this->perSeconds = getTicksPerSecond();
	cout << "1 sec = ";
	printTime(cout,perSeconds);
	cout << endl;
}

ticks BenchTiming::getTicksPerSecond(void) const
{
	timeval tv0;
	timeval tv1;
	ticks t0;
	ticks t1;
	
	//measure
	gettimeofday(&tv0,NULL);
	t0 = getticks();
	sleep(1);
	t1 = getticks();
	gettimeofday(&tv1,NULL);
	
	//compute
	timeval t;
	timersub(&tv1,&tv0,&t);
	double tf = (double)t.tv_sec + (double)t.tv_usec / 1000000.0;
	
	return (double)(t1-t0) / tf;
}

void BenchTiming::start(const char* name, int duration)
{
	this->name = name;
	this->cntEvents = 0;
	this->lastEventStart = 0;
	this->maxEventTime = 0;
	this->minEventTime = 0;
	this->sumEvents = 0;
	ticks durationTicks = (ticks)duration * (ticks)perSeconds;
	this->startTime = getticks();
	this->expectedEnd = startTime + durationTicks;
}

bool BenchTiming::needRun(void) const
{
	return getticks() < expectedEnd;
}

void BenchTiming::eventStart(void)
{
	this->lastEventStart = getticks();
}

void BenchTiming::eventEnd(void)
{
	ticks t = getticks() - lastEventStart;
	
	if (this->cntEvents == 0)
	{
		this->minEventTime = this->maxEventTime = t;
	} else {
		if (t < minEventTime) minEventTime = t;
		if (t> maxEventTime) maxEventTime = t;
	}
	this->cntEvents++;
	this->sumEvents += t;
}

void BenchTiming::end(ostream& out)
{
	ticks t = getticks() - startTime;
	assert(getticks() > expectedEnd);
	
	out << std::setw(20) << name << " : repeated = " << cntEvents << " duration : ";
	printTime(out,t);
	out << " : event : [ ";
	printTime(out,minEventTime);
	out << " , ";
	printTime(out,(double)sumEvents / (double)cntEvents);
	out << " , ";
	printTime(out,maxEventTime);
	out << " ]" << endl;
}

static const char * units[] = {"","K","M","G","T","P"};
void BenchTiming::printTime(ostream& out, ticks value)
{
	double finalValue = value;
	int depth = 0;
	while (finalValue > 100)
	{
		depth++;
		finalValue /= 1000.0;
	}
	out << finalValue << " " << units[depth] << "cycles";
}

int getRand(int min,int max)
{
	return min + (rand() % (max-min));
}

void initAsRandomStack(BacktraceStack & stack)
{
	//vars
	void * buffer[MAX_STACK_SIZE];

	//check not setup
	assert(stack.isValid() == false);
	
	//choose size
	int size = getRand(1,MAX_STACK_SIZE);
	for (int i = 0 ; i < size ; i++)
		buffer[i] = (void*)(unsigned long)rand();
	
	stack.set(buffer,size,STACK_ORDER_ASC);
}

void runTestFlat(BenchTiming & timing,StackSTLHashMap<CallStackInfo> & tracer,BacktraceStack * stacks, const char * name,int duration)
{
	timing.start(name,duration);
	while(timing.needRun())
	{
		//choose random stack
		BacktraceStack & stack = stacks[getRand(0,MAX_STACKS)];
		int reuse = getRand(1,MAX_LOCAL_REUSE);
		
		//if stack not valid, build it
		if (stack.isValid() == false)
			initAsRandomStack(stack);
		
		//measure
		for (int i = 0 ; i < reuse ; i++)
		{
			timing.eventStart();
			tracer.getValueRef(stack).onAllocEvent(16,0);
			timing.eventEnd();
		}
	}
	timing.end(cout);
}

void runTestFlatOld(BenchTiming & timing,SimpleStackTracker & tracer,BacktraceStack * stacks, const char * name,int duration)
{
	timing.start(name,duration);
	while(timing.needRun())
	{
		//choose random stack
		BacktraceStack & stack = stacks[getRand(0,MAX_STACKS)];
		int reuse = getRand(1,MAX_LOCAL_REUSE);
		
		//if stack not valid, build it
		if (stack.isValid() == false)
			initAsRandomStack(stack);
		
		//measure
		for (int i = 0 ; i < reuse ; i++)
		{
			timing.eventStart();
			tracer.getBacktraceInfo(stack).getInfo().onAllocEvent(16,0);
			timing.eventEnd();
		}
	}
	timing.end(cout);
}


void runTestTree(BenchTiming & timing,RLockFreeTree<CallStackInfo> & tracer,BacktraceStack * stacks, const char * name,int duration)
{
	timing.start(name,duration);
	while(timing.needRun())
	{
		//choose random stack
		BacktraceStack & stack = stacks[getRand(0,MAX_STACKS)];
		int reuse = getRand(1,MAX_LOCAL_REUSE);
		
		//if stack not valid, build it
		if (stack.isValid() == false)
			initAsRandomStack(stack);
		
		//measure
		for (int i = 0 ; i < reuse ; i++)
		{
			timing.eventStart();
			tracer.getDataFromStack(stack)->onAllocEvent(16,0);
			timing.eventEnd();
		}
	}
	timing.end(cout);
}

int main(void)
{
	//setup timings
	BenchTiming timing;
	StackSTLHashMap<CallStackInfo> tracer;
	SimpleStackTracker tracerOld;
	RLockFreeTree<CallStackInfo> tracerTree;
	gblInternaAlloc = new SimpleAllocator(true);

	//init random
	srand(time(NULL));
	
	//store random stacks to reuse	
	BacktraceStack * stacks = new BacktraceStack[MAX_STACKS];
	
	int duration = 20;
	printf("Running earch test for %d seconds\n",duration);
	
	runTestFlat(timing,tracer,stacks,"testFlatEmpty",duration);
	runTestFlat(timing,tracer,stacks,"testFlatFull",duration);
	
	runTestFlatOld(timing,tracerOld,stacks,"testFlatOldEmpty",duration);
	runTestFlatOld(timing,tracerOld,stacks,"testFlatOldFull",duration);
	
	runTestTree(timing,tracerTree,stacks,"testTreeEmpty",duration);
	runTestTree(timing,tracerTree,stacks,"testTreeFull",duration);
	return 0;
}
