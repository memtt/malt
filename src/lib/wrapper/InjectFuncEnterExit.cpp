/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 02/2025
*    LICENSE  : CeCILL-C
*    FILE     : ./src/lib/wrapper/InjectFuncEnterExit.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
#include "GlobalState.hpp"
#include "AllocWrapperExtend.hpp"
#include "InjectFuncEnterExit.hpp"

/**********************************************************/
using namespace MALT;

/**********************************************************/
/**
 * Symbol used to capture entering in function when working with GCC/CLANG/ICC 
 * -finstrument-function CFLAG.
 *
 * It need to enable STACK_MODE_USER on profiler object to be usefull. This flag
 * is enabled by setting environnement variable STACK_MODE to "enter-exit" before
 * loading the instrumentation library.
 *
 * @param this_fn Define the name of the current function to exit.
 * @param call_site Define the address which made the call and on which to return.
**/
void __cyg_profile_func_enter (void *this_fn,void *call_site)
{
	//get local TLS and check init
	MALT_WRAPPER_LOCAL_STATE_INIT
	
	//stack tracking
	if (tlsState.status == ALLOC_WRAP_READY || tlsState.status == ALLOC_WRAP_DISABLED)
		localState.profiler->onEnterFunc(this_fn,call_site);
}

/**********************************************************/
/**
 * Symbol used to capture exit of function when working with GCC/CLANG/ICC 
 * -finstrument-function CFLAG. 
 * 
 * It need to enable STACK_MODE_USER on profiler object to be usefull. This flag
 * is enabled by setting environnement variable STACK_MODE to "enter-exit" before
 * loading the instrumentation library.
 *
 * @param this_fn Define the name of the current function to exit.
 * @param call_site Define the address which made the call and on which to return.
**/
void __cyg_profile_func_exit  (void *this_fn,void *call_site)
{
	//get local TLS and check init
	MALT_WRAPPER_LOCAL_STATE_INIT
	
	//stack tracking
	if (tlsState.status == ALLOC_WRAP_READY || tlsState.status == ALLOC_WRAP_DISABLED)
		localState.profiler->onExitFunc(this_fn,call_site);
}
