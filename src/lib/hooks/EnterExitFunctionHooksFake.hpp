/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MATT_ENTER_EXIT_FUNCTION_HOOKS_FAKE_HPP
#define MATT_ENTER_EXIT_FUNCTION_HOOKS_FAKE_HPP

/********************  HEADERS  *********************/
#include <cstdlib>
#include "EnterExitFunctionHooks.hpp"

/***************** USING NAMESPACE ******************/
namespace MATT
{

/*********************  CLASS  **********************/
class EnterExitFunctionHooksFake : public EnterExitFunctionHooks
{
	public:
		EnterExitFunctionHooksFake();
		virtual ~EnterExitFunctionHooksFake(void) {};
		virtual void onEnterFunction ( void* caller, void* function );
		virtual void onExitFunction ( void* caller, void* function );
		virtual bool isEnterExitFunction ( void );
};

};

#endif //MATT_ENTER_EXIT_FUNCTION_HOOKS_FAKE_HPP
