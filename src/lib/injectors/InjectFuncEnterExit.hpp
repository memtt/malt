/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 02/2025
*    LICENSE  : CeCILL-C
*    FILE     : ./src/lib/wrapper/InjectFuncEnterExit.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

#ifndef MALT_INJECT_FUNC_ENTER_EXIT_HPP
#define MALT_INJECT_FUNC_ENTER_EXIT_HPP

/**********************************************************/
//Juste to be sure of the C symbol naming.
extern "C" 
{
	void __cyg_profile_func_enter (void *this_fn,void *call_site);
	void __cyg_profile_func_exit  (void *this_fn,void *call_site);
}

#endif //MALT_INJECT_FUNC_ENTER_EXIT_HPP
