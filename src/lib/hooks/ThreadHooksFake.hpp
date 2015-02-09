/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MATT_THREAD_HOOKS_FAKE_HPP
#define MATT_THREAD_HOOKS_FAKE_HPP

/********************  HEADERS  *********************/
//standard
#include <common/Debug.hpp>
#include "ThreadHooks.hpp"

namespace MATT
{

/*********************  CLASS  **********************/
class ThreadHooksFake : public ThreadHooks
{
	public:
		ThreadHooksFake();
		virtual void onThreadCreate(void);
		virtual void onThreadExit(void);
};

}

#endif //MATT_THREAD_HOOKS_FAKE_HPP
