/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 02/2025
*    LICENSE  : CeCILL-C
*    FILE     : ./src/lib/wrapper/InjectMaqao.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
#include "state/GlobalState.hpp"
#include "WrapperCAlloc.hpp"
#include "InjectFuncEnterExit.hpp"

/**********************************************************/
using namespace MALT;

/**********************************************************/
extern "C" {
	void maqao_enter_function(int fid,const char * funcName);
	void maqao_exit_function(int fid,const char * funcName);
	void maqao_reg_function(int funcId,const char * funcName,const char * file,int line);
}

/**********************************************************/
void maqao_enter_function(int fid,const char * funcName)
{
	__cyg_profile_func_enter((void*)(size_t)fid,(void*)(size_t)fid);
}

/**********************************************************/
void maqao_exit_function(int fid,const char * funcName)
{
	__cyg_profile_func_exit((void*)(size_t)fid,(void*)(size_t)fid);
}

/**********************************************************/
void maqao_reg_function(int funcId,const char * funcName,const char * file,int line)
{
	//check init
	if (gblState.status == ALLOC_WRAP_NOT_READY)
		free(NULL);

	//mark stack mode to be automatic
	gblState.options->stackMode = STACK_MODE_ENTER_EXIT_FUNC;
	
	//reg
	gblState.profiler->registerMaqaoFunctionSymbol(funcId,funcName,file,line);
}
