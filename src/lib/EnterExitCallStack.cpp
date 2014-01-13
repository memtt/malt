/********************  HEADERS  *********************/
#include "EnterExitCallStack.h"
#include <cstring>
#include <cstdio>
#include <json/JsonState.h>

#define MAX_SIZE 32

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
	else if (stack[size-1] == funcAddr)
		return;

	//check realSize
	if (++realSize > MAX_SIZE)
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

	if (realSize-- >MAX_SIZE)
		return;

	if (size > 0)
		size--;

	assert(funcAddr == stack[size]);
}
