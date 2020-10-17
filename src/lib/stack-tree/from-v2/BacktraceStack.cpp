/*****************************************************
             PROJECT  : MALT
             VERSION  : 1.2.2
             DATE     : 10/2020
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
//standard
#include <cstdio>
#include <cassert>
#include <cstring>
//internals
#include <common/Debug.hpp>
//portability
#include <tools/Backtrace.hpp>
//current
#include "BacktraceStack.hpp"

/********************  MACROS  **********************/
/**
 * Limit the maximum size of the backtraces we extract to keep resonnable things.
 * @TODO Move this into the option class to support dynamic definition.
**/
#define MALT_CALL_STACK_MAX (128*1024)

/*******************  NAMESPACE  ********************/
namespace MALTV2 
{

/*******************  FUNCTION  *********************/
/**
 * Constructor to setup the ordering mode compatible with backtrace().
**/
BacktraceStack::BacktraceStack(void)
	:Stack(STACK_ORDER_ASC)
{
}

/*******************  FUNCTION  *********************/
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
		int loadedSize = MALT::Backtrace::backtrace(this->stack,this->memSize);

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
	for (int i = 0 ; i < this->size ; i++)
		this->stack[i] = (void*)((size_t)this->stack[i] - 1);
}

}
