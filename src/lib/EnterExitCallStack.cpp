/********************  HEADERS  *********************/
#include "EnterExitCallStack.h"
#include <cstring>
#include <cstdio>

/*******************  FUNCTION  *********************/
EnterExitCallStack::EnterExitCallStack ( void )
	:Stack(STACK_ORDER_DESC)
{
}

/*******************  FUNCTION  *********************/
void EnterExitCallStack::enterFunction ( void* funcAddr )
{
	//check default
	if (stack == NULL)
		this->grow();
	else if (stack[size-1] == funcAddr)
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

	if (size > 0)
		size--;

	assert(funcAddr == stack[size]);
}
