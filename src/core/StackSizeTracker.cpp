/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
#include <cassert>
#include <cstdio>
#include "StackSizeTracker.hpp"
#include <cycle.h>

/*******************  NAMESPACE  ********************/
namespace MATT
{

/*******************  FUNCTION  *********************/
StackSizeTracker::StackSizeTracker(void)
:stack(32,512,32)
{
	this->min = 0;
	this->cur = 0;
	this->stack.push_back(0);
}

/*******************  FUNCTION  *********************/
void StackSizeTracker::enter(void)
{
	//get stack size
	unsigned long crbp;
	unsigned long crsp;
	
	//read counters
	asm("movq %%rbp,%0" : "=r"(crbp));
	asm("movq %%rsp,%0" : "=r"(crsp));
	
	//debug
	//printf("matt %llu %lu\n",getticks(),crsp);
	
	//push
	this->stack.push_back(crsp);
	this->cur = crsp;
	
	//min
	if (this->min == 0)
		this->min = this->cur;
}

/*******************  FUNCTION  *********************/
void StackSizeTracker::exit(void)
{
	this->stack.pop();
}

/*******************  FUNCTION  *********************/
long unsigned int StackSizeTracker::getSize(void) const
{
	return this->cur - this->min;
}

/*******************  FUNCTION  *********************/
StackSizeTracker& StackSizeTracker::operator=(const StackSizeTracker& orig)
{
	this->stack.set(orig.stack);
	return *this;
}

}
