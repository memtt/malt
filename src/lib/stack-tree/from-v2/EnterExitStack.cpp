/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.2
*    DATE     : 06/2023
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/stack-tree/from-v2/EnterExitStack.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2015
***********************************************************/
/*****************************************************
             PROJECT  : MALT
             VERSION  : 1.2.2
             DATE     : 06/2023
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
//standard
#include <cstring>
#include <cstdio>
//from htopml
#include <json/JsonState.h>
//internals
#include "EnterExitStack.hpp"

/*******************  NAMESPACE  ********************/
namespace MALTV2
{

/*******************  FUNCTION  *********************/
/**
 * Constructor for the enter-exit mode. It mostly setup the DESC convention for the underlying
 * generic stack implementation.
**/
EnterExitStack::EnterExitStack ( void )
	:Stack(STACK_ORDER_DESC)
{
	this->realSize = 0;
}

}
