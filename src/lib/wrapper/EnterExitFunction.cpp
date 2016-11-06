#include <hooks/EnterExitFunctionHooks.hpp>
#include <cstdio>

/*********************  STRUCT  *********************/
//Juste to be sure of the C symbol naming.
extern "C" 
{
	void __cyg_profile_func_enter (void *this_fn,void *call_site);
	void __cyg_profile_func_exit  (void *this_fn,void *call_site);
}

/*******************  FUNCTION  *********************/
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
	MATT::EnterExitFunctionHooks * hooks = NULL;
	bool isEnterExit = false;
	
	hooks = MATT::enterExitFunctionHookInit();
	if (hooks != NULL)
		isEnterExit = hooks->isEnterExitFunction();

// 	if (isEnterExit)
		hooks->onEnterFunction(this_fn,call_site);
}

/*******************  FUNCTION  *********************/
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
	MATT::EnterExitFunctionHooks * hooks = NULL;
	bool isEnterExit = false;
	
	hooks = MATT::enterExitFunctionHookInit();
	if (hooks != NULL)
		isEnterExit = hooks->isEnterExitFunction();

// 	if (isEnterExit)
		hooks->onExitFunction(this_fn,call_site);
}
