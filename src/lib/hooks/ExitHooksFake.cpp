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
#include "ExitHooksFake.hpp"

namespace MATT
{

/*******************  FUNCTION  *********************/
ExitHooksFake::ExitHooksFake()
{

}

/*******************  FUNCTION  *********************/
void ExitHooksFake::onExit(void)
{
	MATT_INFO("onExit()");
}

}
