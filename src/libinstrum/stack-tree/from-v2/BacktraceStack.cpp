/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.3.1
*    DATE     : 02/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/stack-tree/from-v2/BacktraceStack.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2015
*    AUTHOR   : Bastien Levasseur - 2024
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
//standard
#include <cstdio>
#include <cassert>
#include <cstring>
//internals
#include <common/Debug.hpp>
#include <common/SimpleAllocator.hpp>
//portability
#include <tools/Backtrace.hpp>
//current
#include "BacktraceStack.hpp"

/**********************************************************/
/**
 * Limit the maximum size of the backtraces we extract to keep resonnable things.
 * @TODO Move this into the option class to support dynamic definition.
**/
#define MALT_CALL_STACK_MAX (128*1024)

/**********************************************************/
namespace MALTV2 
{

/**********************************************************/
/**
 * Constructor to setup the ordering mode compatible with backtrace().
**/
BacktraceStack::BacktraceStack(void)
	:Stack(STACK_ORDER_ASC)
{
}

/**********************************************************/
/**
 * Destructor.
**/
BacktraceStack::~BacktraceStack(void)
{
	MALT_FREE(this->stackBuffer);
}

/**********************************************************/
/**
 * Load the current backtrace. This function will automatically grow the internal buffer
 * to adapt to the size needed to store the full call depth.
**/
void BacktraceStack::loadCurrentStack(void)
{
	this->stack = this->mem;
	if (this->stack == NULL)
		this->grow();
	
	//try to load in current buffer, if not realloc and retry
	bool retry;
	do {
		//try to load with current buffer
		//int loadedSize = backtrace(this->stack,this->memSize);
		//int loadedSize = GetStackTrace(this->stack,this->memSize,0);

		//Check if the LangAddress representation is the same as a C pointer representation? With the far left bit at zero.
		assert(MALT::DOMAIN_C == 0);
		assert(sizeof(MALT::LangAddress) == sizeof(void*));
		assert(sizeof(this->stack[0]) == sizeof(void*));
		
		int loadedSize = MALT::Backtrace::backtrace(this->stackBuffer,this->memSize);

		assert(loadedSize <= this->memSize);
		assert(loadedSize > 0);

		//miss some entries, need to grow the buffer
		if (loadedSize >= MALT_CALL_STACK_MAX)
		{
			static bool once = false;
			if (!once)
			{
				once = true;
				MALT_ERROR("Caution, need to cut some call stacks !");
			}
			break;
		} else if (loadedSize == this->memSize) {
			this->grow();
			retry = true;
		} else {
			this->size = loadedSize;
			retry = false;
		}
	} while(retry);
	
	//fix addresses, backtrace return next instruction, by substracting 1 we go to the middle
	//of the previous instruction. addr2line is ok with non exact addresses under linux at least.
	for (int i = 0 ; i < this->size ; i++) {
		this->stack[i].set(MALT::DOMAIN_C, this->stackBuffer[i]);
		if (this->stack[i].isNULL() == false)
			this->stack[i] -= 1;
	}
}

/**********************************************************/
/**
 * Realloc the current buffer to backtrace().
 */
void BacktraceStack::onGrow(size_t newSize)
{
	this->stackBuffer = (void**)MALT_REALLOC(this->stackBuffer,this->memSize * sizeof(void*));
}

}
