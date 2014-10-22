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
//libunwind
#include <libunwind.h>
//internals
#include <common/Debug.hpp>
#include <stacks/BacktraceStack.hpp>

/********************  MACROS  **********************/
/**
 * Limit the maximum size of the backtraces we extract to keep resonnable things.
 * @TODO Move this into the option class to support dynamic definition.
**/
#define MATT_CALL_STACK_MAX (128*1024)

int GetStackTrace(void** result, int max_depth, int skip_count);

/*******************  NAMESPACE  ********************/
namespace MATT 
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
int libunwindbactrace(void** buffer,int maxDepth)
{
	unw_cursor_t    cursor;
	unw_context_t   context;

	unw_getcontext(&context);
	unw_init_local(&cursor, &context);

	int depth = 0;
	while (unw_step(&cursor) > 0 && depth < maxDepth) {
		unw_word_t  pc;
// 		char        fname[64];

		unw_get_reg(&cursor, UNW_REG_IP, &pc);

//         fname[0] = '\0';
//         (void) unw_get_proc_name(&cursor, fname, sizeof(fname), &offset);

//         printf ("%p : (%s+0x%x) [%p]\n", pc, fname, offset, pc);
		if (unw_step(&cursor) > 0)
		{
			buffer[depth] = (void*)pc;
			depth++;
		}
	}
	return depth;
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
		int loadedSize = libunwindbactrace(this->stack,this->memSize);

		assert(loadedSize <= this->memSize);
		assert(loadedSize > 0);

		//miss some entries, need to grow the buffer
		if (loadedSize >= MATT_CALL_STACK_MAX)
		{
			static bool once = false;
			if (!once)
			{
				once = true;
				MATT_ERROR("Caution, need to cut some call stacks !");
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
