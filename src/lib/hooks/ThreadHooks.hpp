/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MATT_THREAD_HOOKS_HPP
#define MATT_THREAD_HOOKS_HPP

/***************** USING NAMESPACE ******************/
namespace MATT
{

/*********************  CLASS  **********************/
class ThreadHooks
{
	public:
		virtual ~ThreadHooks(void) {};
		virtual void onThreadCreate(void) = 0;
		virtual void onThreadExit(void) = 0;
};

/*******************  FUNCTION  *********************/
ThreadHooks * threadHookInit(void);

};

#endif //MATT_THREAD_HOOKS_HPP