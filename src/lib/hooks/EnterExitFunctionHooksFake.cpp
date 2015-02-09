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
#include "EnterExitFunctionHooksFake.hpp"

namespace MATT
{

/*******************  FUNCTION  *********************/
EnterExitFunctionHooksFake::EnterExitFunctionHooksFake()
{

}

/*******************  FUNCTION  *********************/
void EnterExitFunctionHooksFake::onEnterFunction ( void* caller, void* function )
{
	MATT_INFO_ARG("enter_function( %1 , %2 )").arg(caller).arg(function).end();
}

/*******************  FUNCTION  *********************/
void EnterExitFunctionHooksFake::onExitFunction ( void* caller, void* function )
{
	MATT_INFO_ARG("exit_function( %1 , %2 )").arg(caller).arg(function).end();
}

/*******************  FUNCTION  *********************/
bool EnterExitFunctionHooksFake::isEnterExitFunction ( void )
{
	return true;
}

}
