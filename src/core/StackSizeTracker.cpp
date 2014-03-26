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
	this->base = 0;
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
	
	//push
	this->stack.push_back(crsp);
	this->cur = crsp;
	
	//min
	if (this->base == 0 || this->cur > this->base)
		this->base = this->cur;
	
	//debug
	//printf("matt %llu , %lu , %lu , %lu\n",getticks(),this->base,crsp,getSize());
}

/*******************  FUNCTION  *********************/
void StackSizeTracker::exit(void)
{
	this->stack.pop();
}

/*******************  FUNCTION  *********************/
long unsigned int StackSizeTracker::getSize(void) const
{
	return this->base - this->cur;
}

/*******************  FUNCTION  *********************/
StackSizeTracker& StackSizeTracker::operator=(const StackSizeTracker& orig)
{
	this->stack.set(orig.stack);
	this->base = orig.base;
	this->cur = orig.cur;
	return *this;
}

/*******************  FUNCTION  *********************/
void convertToJson(htopml::JsonState& json, const StackSizeTracker& value)
{
	int size = value.stack.getSize();
	json.openArray();
	for (int i = size - 1 ; i >= 0 ; i--)
	{
		if (value.stack[i] == 0)
			json.printValue(0);
		else
			json.printValue(value.base - value.stack[i]);
	}
	json.closeArray();
}

}
