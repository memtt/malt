/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.6
*    DATE     : 10/2015
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/stack-tree/from-v2/EnterExitStack.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2015
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
