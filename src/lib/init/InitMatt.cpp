/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
#include "InitMatt.hpp"

/*******************  FUNCTION  *********************/
namespace MATT
{

/********************  GLOBALS  *********************/
static InitMatt * gblMatt;

/*******************  FUNCTION  *********************/
ThreadHooks * threadHookInit(void)
{
	return gblMatt;
}

/*******************  FUNCTION  *********************/
MallocHooks * mallocHookInit(void)
{
	return gblMatt;
}

/*******************  FUNCTION  *********************/
MmapHooks * mmapHookInit(void)
{
	return gblMatt;
}

/*******************  FUNCTION  *********************/
ExitHooks * exitHookInit(void)
{
	return gblMatt;
}

}