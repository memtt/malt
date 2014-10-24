/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
#include "BacktraceLibunwind.hpp"
//libunwind
#include <libunwind.h>

/*******************  NAMESPACE  ********************/
namespace MATT
{

/*******************  FUNCTION  *********************/
int BacktraceLibunwind::backtrace(void** buffer, int maxDepth)
{
	unw_cursor_t    cursor;
	unw_context_t   context;

	unw_getcontext(&context);
	unw_init_local(&cursor, &context);

	int depth = 0;
	
	//skip first for compat with glibc backtrace and loop
	while(depth < maxDepth && unw_step(&cursor) > 0) {
		unw_word_t  pc;
		//char        fname[64];

		unw_get_reg(&cursor, UNW_REG_IP, &pc);

		//fname[0] = '\0';
		//(void) unw_get_proc_name(&cursor, fname, sizeof(fname), &offset);
		//printf ("%p : (%s+0x%x) [%p]\n", pc, fname, offset, pc);
		buffer[depth] = (void*)pc;
		depth++;
	};

	//remove 0xfffffffff
	if (depth < maxDepth)
		depth--; 

	return depth;
}

}
