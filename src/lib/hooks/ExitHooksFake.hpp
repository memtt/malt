/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 02/2015
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MATT_EXIT_HOOKS_FAKE_HPP
#define MATT_EXIT_HOOKS_FAKE_HPP

/********************  HEADERS  *********************/
#include <cstdlib>
#include "ExitHooks.hpp"

/***************** USING NAMESPACE ******************/
namespace MATT
{

/*********************  CLASS  **********************/
class ExitHooksFake : public ExitHooks
{
	public:
		ExitHooksFake();
		virtual ~ExitHooksFake(void) {};
		virtual void onExit(void);
};

};

#endif //MATT_EXIT_HOOKS_FAKE_HPP
