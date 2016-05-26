/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 02/2015
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MATT_EXIT_HOOKS_HPP
#define MATT_EXIT_HOOKS_HPP

/********************  HEADERS  *********************/
#include <cstdlib>

/***************** USING NAMESPACE ******************/
namespace MATT
{

/*********************  CLASS  **********************/
class ExitHooks
{
	public:
		virtual ~ExitHooks(void) {};
		virtual void onExit(void) = 0;
};

/*******************  FUNCTION  *********************/
ExitHooks * exitHookInit(void);

}

#endif //MATT_EXIT_HOOKS_HPP