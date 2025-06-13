/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.3.0
*    DATE     : 11/2014
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/stacks/EnterExitStack.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (ECR) - 2014
*    AUTHOR   : Sébastien Valat - 2014
***********************************************************/

/**********************************************************/
//standard
#include <cstring>
#include <cstdio>
//from htopml
#include <json/JsonState.h>
//internals
#include <stacks/EnterExitStack.hpp>

/**********************************************************/
namespace MALT
{

/**********************************************************/
/**
 * Constructor for the enter-exit mode. It mostly setup the DESC convention for the underlying
 * generic stack implementation.
**/
EnterExitStack::EnterExitStack ( void )
	:Stack(STACK_ORDER_DESC)
{
}

}
