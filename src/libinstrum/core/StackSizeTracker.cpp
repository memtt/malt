/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.3.0
*    DATE     : 02/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/core/StackSizeTracker.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (ECR) - 2014
*    AUTHOR   : Sébastien Valat - 2020 - 2021
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
#include <cassert>
#include <cstdio>
#include "StackSizeTracker.hpp"
#include <portability/LinuxProcMapReader.hpp>
#include <portability/Clock.hpp>

/**********************************************************/
namespace MALT
{

/**********************************************************/
/**
 * Constructor of the stack size tracker.
**/
StackSizeTracker::StackSizeTracker(void)
                 :stack(128,2048,32)
{
	this->base = 0;
	this->cur = 0;
	this->mapLower = 0;
	this->mapUpper = 0;
	this->stack.push_back(0);
}

/**********************************************************/
/**
 * To be called when we enter in a function, it will use assembly instruction
 * to extract the current stack pointer and compute the stack size consumed by
 * the parent function.
**/
void StackSizeTracker::enter(void)
{
	//get stack size
	unsigned long crsp;
	
	//read counters
	#if defined(__x86_64__)
		asm("movq %%rsp,%0" : "=r"(crsp));
	#elif defined(__i386__)
		asm("mov %%esp,%0" : "=r"(crsp));
	#elif defined(__aarch__) || defined(__aarch64__)
		asm("mov %0, sp" : "=r"(crsp));
	#else
		MALT_FATAL("Enter-exit mode is not supported for this architecture !");
		#warning "Arch not supported, stack size tracking will be ignored, all rest will work fine, this is optional."
	#endif
	
	this->enter(crsp);
}

/**********************************************************/
/**
 * Enter in the stack by giving the stack size from the caller.
 */
void StackSizeTracker::enter(size_t stackPointer)
{
	//push
	this->stack.push_back(stackPointer);
	this->cur = stackPointer;
	
	//min
	if (this->base == 0 || this->cur > this->base)
		this->base = this->cur;
	
	//laod upper/lower limits by checking with proc map
	//if (this->mapLower == 0 && this->mapUpper ==0)
	//	this->loadMapping();
	
	//debug
	//printf("malt %llu , %zu , %zu , %zu\n",Clock::getticks(),this->base,crsp,getSize());
}

/**********************************************************/
/**
 * Function used to extract mapLower and mapUpper from /proc/self/maps.
**/
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

/**********************************************************/
/**
 * To be called when exiting a function to pop the stack size entry.
**/
void StackSizeTracker::exit(void)
{
	this->stack.pop();
}

/**********************************************************/
/**
 * Return the current size of the stack by a diff between the base address
 * and the current one.
**/
long unsigned int StackSizeTracker::getSize(void) const
{
	return this->base - this->cur;
}

/**********************************************************/
/**
 * Copy operator.
 * @param origin Reference to the original stack size tracker.
**/
StackSizeTracker& StackSizeTracker::operator=(const StackSizeTracker& orig)
{
	this->stack.set(orig.stack);
	this->base = orig.base;
	this->cur = orig.cur;
	this->mapLower = orig.mapLower;
	this->mapUpper = orig.mapUpper;
	return *this;
}

/**********************************************************/
/**
 * Conver the stack size tracker into json format for the final profile file.
 * @param json Reference to the json state to make the conversion.
 * @param value Reference to the object to convert.
**/
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

/**********************************************************/
/**
 * Return the total size using the mapUpper/mapLower from /proc/self/maps.
**/
long unsigned int StackSizeTracker::getTotalSize(void) const
{
	return mapUpper - mapLower;
}

}
