/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
#include "InitMatt.hpp"
#include <hooks/EnterExitFunctionHooks.hpp>

/*******************  FUNCTION  *********************/
namespace MATT
{

/********************  GLOBALS  *********************/
static bool gblMattWasInit = false;
static InitMatt gblMatt;

/*******************  FUNCTION  *********************/
ThreadHooks * threadHookInit(void)
{
	if (!gblMattWasInit)
	{
		gblMatt.init();
		gblMattWasInit = true;
	}
	return gblMatt;
}

/*******************  FUNCTION  *********************/
MallocHooks * mallocHookInit(void)
{
	if (!gblMattWasInit)
	{
		gblMatt.init();
		gblMattWasInit = true;
	}
	return gblMatt;
}

/*******************  FUNCTION  *********************/
MmapHooks * mmapHookInit(void)
{
	if (!gblMattWasInit)
	{
		gblMatt.init();
		gblMattWasInit = true;
	}
	return gblMatt;
}

/*******************  FUNCTION  *********************/
ExitHooks * exitHookInit(void)
{
	if (!gblMattWasInit)
	{
		gblMatt.init();
		gblMattWasInit = true;
	}
	return gblMatt;
}

/*******************  FUNCTION  *********************/
EnterExitFunctionHooks * enterExitFunctionHooks(void)
{
	if (!gblMattWasInit)
	{
		gblMatt.init();
		gblMattWasInit = true;
	}
	return gblMatt;
}

}
