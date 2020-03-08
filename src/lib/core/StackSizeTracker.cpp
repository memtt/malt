/*****************************************************
             PROJECT  : MALT
             VERSION  : 1.2.0-dev
             DATE     : 08/2019
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
#include <cassert>
#include <cstdio>
#include "StackSizeTracker.hpp"
#include <portability/LinuxProcMapReader.hpp>
#include <cycle.h>

/*******************  NAMESPACE  ********************/
namespace MALT
{

/*******************  FUNCTION  *********************/
StackSizeTracker::StackSizeTracker(void)
:stack(128,2048,32)
{
	this->base = 0;
	this->cur = 0;
	this->mapLower = 0;
	this->mapUpper = 0;
	this->stack.push_back(0);
}

/*******************  FUNCTION  *********************/
void StackSizeTracker::enter(void)
{
	//get stack size
	unsigned long crbp;
	unsigned long crsp;
	
	//read counters
	#ifdef __x86_64__
		asm("movq %%rbp,%0" : "=r"(crbp));
		asm("movq %%rsp,%0" : "=r"(crsp));
	#elif __i386__
		asm("mov %%ebp,%0" : "=r"(crbp));
		asm("mov %%esp,%0" : "=r"(crsp));
	#else
		MALT_FATAL("Enter-exit mode is not supported for this architecture !");
		#warning "Arch not supported, stack size tracking will be ignored, all rest will work fine, this is optional."
	#endif
	
	//push
	this->stack.push_back(crsp);
	this->cur = crsp;
	
	//min
	if (this->base == 0 || this->cur > this->base)
		this->base = this->cur;
	
	//laod upper/lower limits by checking with proc map
	//if (this->mapLower == 0 && this->mapUpper ==0)
	//	this->loadMapping();
	
	//debug
	//printf("malt %llu , %lu , %lu , %lu\n",getticks(),this->base,crsp,getSize());
}

/*******************  FUNCTION  *********************/
void StackSizeTracker::loadMapping(void)
{
	//read proc map
	LinuxProcMapReader procMap;
	procMap.load();
	
	//get entry
	const LinuxProcMapEntry * entry = procMap.getEntry((void*)this->base);
	
	//not found
	if (entry == NULL)
	{
		MALT_WARNING("Failed to found stack mapping in /proc/map for !");
	} else {
		//copy info
		this->mapLower = (unsigned long)entry->lower;
		this->mapUpper = (unsigned long)entry->upper;
	}
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
	this->mapLower = orig.mapLower;
	this->mapUpper = orig.mapUpper;
	return *this;
}

/*******************  FUNCTION  *********************/
void convertToJson(htopml::JsonState& json, const StackSizeTracker& value)
{
	int size = value.stack.size();
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

/*******************  FUNCTION  *********************/
long unsigned int StackSizeTracker::getTotalSize(void) const
{
	return mapUpper - mapLower;
}

}
