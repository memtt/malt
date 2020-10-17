/*****************************************************
             PROJECT  : MALT
             VERSION  : 1.2.2
             DATE     : 10/2020
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
//malt config
#include <config.h>
//malt common
#include <common/Debug.hpp>
//libunwind
#ifdef MALT_HAVE_LIBUNWIND
	#include <libunwind.h>
#endif //MALT_HAVE_LIBUNWIND
//local
#include "BacktraceLibunwind.hpp"

/*******************  NAMESPACE  ********************/
namespace MALT
{

/*******************  FUNCTION  *********************/
#ifdef MALT_HAVE_LIBUNWIND
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
#else //MALT_HAVE_LIBUNWIND
	#warning Missing libunwind, disable optional support.
	int BacktraceLibunwind::backtrace(void** buffer, int maxDepth)
	{
		MALT_FATAL("Libunwind support wasn't compiled for you version of MALT please recompile or use the default glibc backtracing method !");
		return 0;
	}
#endif //MALT_HAVE_LIBUNWIND

}
