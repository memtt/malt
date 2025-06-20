/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.3.1
*    DATE     : 03/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/injectors/InjectFuncEnterExit.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
#include "state/LazyEnv.hpp"
#include "wrappers/WrapperCAlloc.hpp"
#include "InjectFuncEnterExit.hpp"
#include "portability/Visibility.hpp"

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
DLL_PUBLIC void __cyg_profile_func_enter (void *this_fn,void *call_site)
{
	//get local TLS and check init
	LazyEnv env;
	
	//stack tracking
	if (env.isMaltReady())
		env.getLocalProfiler().onEnterFunc(LangAddress(DOMAIN_C, this_fn),LangAddress(DOMAIN_C, call_site));
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
DLL_PUBLIC void __cyg_profile_func_exit  (void *this_fn,void *call_site)
{
	//get local TLS and check init
	LazyEnv env;
	
	//stack tracking
	if (env.isMaltReady())
		env.getLocalProfiler().onExitFunc(LangAddress(DOMAIN_C, this_fn),LangAddress(DOMAIN_C, call_site));
}
