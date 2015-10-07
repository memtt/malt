/*****************************************************
             PROJECT  : MALT
             VERSION  : 0.1.0-dev
             DATE     : 02/2015
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
