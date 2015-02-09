/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
//standard
#include <common/Debug.hpp>
#include "ThreadHooksFake.hpp"

namespace MATT
{

/*******************  FUNCTION  *********************/
ThreadHooksFake::ThreadHooksFake()
{
	MATT_INFO("Init thread hooks");
}

/*******************  FUNCTION  *********************/
void ThreadHooksFake::onThreadCreate(void)
{
	MATT_INFO("Thread create");
}

/*******************  FUNCTION  *********************/
void ThreadHooksFake::onThreadExit(void)
{
	MATT_INFO("Thread exit");
}

}
