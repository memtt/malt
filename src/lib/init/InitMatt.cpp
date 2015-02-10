/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 02/2015
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
#include <analysis/ProcessLevelAnalysis.hpp>
#include <analysis/ThreadLevelAnalysis.hpp>
#include <hooks/EnterExitFunctionHooks.hpp>

/*******************  FUNCTION  *********************/
namespace MATT
{

/********************  GLOBALS  *********************/
static ProcessLevelAnalysis * gblMatt = NULL;
static __thread ThreadLevelAnalysis * tlsMatt = NULL;

/*******************  FUNCTION  *********************/
ThreadHooks * threadHookInit(void)
{
	if (gblMatt == NULL)
		gblMatt = new ProcessLevelAnalysis();
	return gblMatt;
}

/*******************  FUNCTION  *********************/
MallocHooks * mallocHookInit(void)
{
	ThreadLevelAnalysis * tls = tlsMatt;
	if (gblMatt == NULL)
		gblMatt = new ProcessLevelAnalysis();
	if (tls == NULL)
		tls = tlsMatt = gblMatt->getNewThreadLevelAnalysis();
	return tls;
}

/*******************  FUNCTION  *********************/
MmapHooks * mmapHookInit(void)
{
	if (gblMatt == NULL)
		gblMatt = new ProcessLevelAnalysis();
	return gblMatt;
}

/*******************  FUNCTION  *********************/
ExitHooks * exitHookInit(void)
{
	if (gblMatt == NULL)
		gblMatt = new ProcessLevelAnalysis();
	return gblMatt;
}

/*******************  FUNCTION  *********************/
EnterExitFunctionHooks * enterExitFunctionHooks(void)
{
	ThreadLevelAnalysis * tls = tlsMatt;
	if (gblMatt == NULL)
		gblMatt = new ProcessLevelAnalysis();
	if (tls == NULL)
		tls = tlsMatt = gblMatt->getNewThreadLevelAnalysis();
	return tls;
}

}
