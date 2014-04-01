/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
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
#include "EnterExitCallStack.hpp"

/********************  MACROS  **********************/
#define MAX_SIZE (1024*1204)

/*******************  NAMESPACE  ********************/
namespace MATT
{

/*******************  FUNCTION  *********************/
EnterExitCallStack::EnterExitCallStack ( void )
	:Stack(STACK_ORDER_DESC)
{
	this->realSize = 0;
}

/*******************  FUNCTION  *********************/
void EnterExitCallStack::enterFunction ( void* funcAddr )
{
	//check default
	if (stack == NULL)
		this->grow();

	//check realSize
	size_t tmp = ++realSize;
	if (tmp > MAX_SIZE)
		return;
	
	//check size
	if (size == memSize || stack == NULL)
		this->grow();
		
	//update
	this->stack[size++] = funcAddr;
	
	//errors
	assert(size <= memSize);
}

/*******************  FUNCTION  *********************/
void EnterExitCallStack::exitFunction ( void* funcAddr )
{
	assert(size > 0);

	size_t tmp = realSize--;
	if (tmp >MAX_SIZE)
		return;

	if (size > 0)
		size--;

	assert(funcAddr == stack[size]);
}

}
