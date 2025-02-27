/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 10/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/stacks/BacktraceStack.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2014 - 2024
*    AUTHOR   : Sébastien Valat (ECR) - 2014
***********************************************************/

/**********************************************************/
//standard
#include <cstdio>
#include <cassert>
#include <cstring>
//internals
#include <common/Debug.hpp>
//portability
#include <tools/Backtrace.hpp>
#include "../tools/BacktraceGlibc.hpp"
//current
#include <stacks/BacktraceStack.hpp>

/**********************************************************/
/**
 * Limit the maximum size of the backtraces we extract to keep resonnable things.
 * @TODO Move this into the option class to support dynamic definition.
**/
#define MALT_CALL_STACK_MAX (128*1024)

/**********************************************************/
namespace MALT 
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
		assert(DOMAIN_C == 0);
		assert(sizeof(LangAddress) == sizeof(void*));
		assert(sizeof(this->stack[0]) == sizeof(void*));
		
		assert(this->stackBuffer != nullptr);
		int loadedSize = Backtrace::backtrace(this->stackBuffer,this->memSize);

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
		this->stack[i].set(DOMAIN_C, this->stackBuffer[i]);
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
	assert(newSize = this->memSize);
	this->stackBuffer = (void**)MALT_REALLOC(this->stackBuffer,newSize * sizeof(void*));
}

/**********************************************************/
/**
 * This function help to auto detect the compiler inlining which append on new
 * GCC linker. Compare using the object bactrace function and the direct call
 * to backtrace function so we can compare if it has been inlined or not.
 * @return Return the delta to apply to the default value in AllocWrapper.cpp.
**/
int BacktraceStack::getBactraceSkipOptimDelta(void)
{
	//make backtrace
	this->loadCurrentStack();
	int sizeLoadCurrent = getSize();


	//Check if the LangAddress representation is the same as a C pointer representation? With the far left bit at zero.
	assert(DOMAIN_C == 0);
	assert(sizeof(LangAddress) == sizeof(void*));
	assert(sizeof(this->stack[0]) == sizeof(void*));
		

	//dirct call
	int sizeDirect = BacktraceGlibc::backtrace(stackBuffer,memSize);

	//calc delta
	int delta = sizeLoadCurrent - sizeDirect;

	//delta==1 => release mode with gcc linker level inlining
	//delta==2 => debig mode without inlining
	if (delta == 1) {
		return 0;
	} else if (delta == 2) {
		return 2;
	} else {
		MALT_WARNING_ARG("Cannot determine compiler inlining type for backtrace"
			"fix, got %1, you might have to adjust with -o stack:skip=X to find"
			" the right value. Using a safe value of 0 for delta this might "
			"capture malt internal details.")
			.arg(delta)
			.end();
	}

	//return
	return 0;
}

}
