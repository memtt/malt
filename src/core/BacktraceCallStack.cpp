/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
//standard
#include <cstdio>
#include <cassert>
#include <cstring>
//GNU specific
#include <execinfo.h>
//internals
#include "BacktraceCallStack.hpp"

/********************  MACROS  **********************/
#define CALL_STACK_MAX (128*1024)

/*******************  NAMESPACE  ********************/
namespace MATT 
{

/*******************  FUNCTION  *********************/
BacktraceCallStack::BacktraceCallStack(void)
	:Stack(STACK_ORDER_ASC)
{
}

/*******************  FUNCTION  *********************/
void BacktraceCallStack::loadCurrentStack(void)
{
	if (this->stack == NULL)
		this->grow();
	
	//try to load in current buffer, if not realloc and retry
	bool retry;
	do {
		//try to load with current buffer
		int loadedSize = backtrace(this->stack,this->memSize);
		assert(loadedSize <= this->memSize);
		assert(loadedSize > 0);

		//miss some entries, need to grow the buffer
		if (loadedSize >= CALL_STACK_MAX)
		{
			static bool once = false;
			if (!once)
			{
				once = true;
				puts("Caution, need to cut some call stacks !");
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
