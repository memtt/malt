/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MATT_ENTER_EXIT_FUNCTION_HOOKS_HPP
#define MATT_ENTER_EXIT_FUNCTION_HOOKS_HPP

/********************  HEADERS  *********************/
#include <cstdlib>

/***************** USING NAMESPACE ******************/
namespace MATT
{

/*********************  CLASS  **********************/
class EnterExitFunctionHooks
{
	public:
		virtual ~EnterExitFunctionHooks(void) {};
		virtual void onEnterFunction(void * caller,void * function) = 0;
		virtual void onExitFunction(void * caller,void * function) = 0;
		virtual bool isEnterExitFunction(void) = 0;
};

/*******************  FUNCTION  *********************/
EnterExitFunctionHooks * enterExitFunctionHookInit(void);

};

#endif //MATT_ENTER_EXIT_FUNCTION_HOOKS_HPP