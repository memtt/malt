/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.3
*    DATE     : 10/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/stack-tree/from-v2/EnterExitStack.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2015 - 2024
***********************************************************/

/**********************************************************/
//standard
#include <cstring>
#include <cstdio>
//from htopml
#include <json/JsonState.h>
//internals
#include "EnterExitStack.hpp"

/**********************************************************/
namespace MALTV2
{

/**********************************************************/
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
