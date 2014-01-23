/********************  HEADERS  *********************/
//extern
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cassert>
#include <json/JsonState.h>
//locals
#include "Stack.h"

/********************  MACROS  **********************/
#define CALL_STACK_DEFAULT_SIZE   32
#define CALL_STACK_GROW_THRESHOLD 1024

/*******************  NAMESPACE  ********************/
namespace ATT
{

/*******************  FUNCTION  *********************/
Stack::Stack ( StackOrder order )
{
	this->order   = order;
	this->stack   = NULL;
	this->size    = 0;
	this->memSize = 0;
}

/*******************  FUNCTION  *********************/
Stack::Stack(void** stack, int size,StackOrder order)
{
	this->order   = order;
	this->stack   = NULL;
	this->size    = 0;
	this->memSize = 0;
	this->set(stack,size,order);
}

/*******************  FUNCTION  *********************/
Stack::Stack ( const Stack& orig )
{
	this->order   = orig.order;
	this->stack   = NULL;
	this->size    = 0;
	this->memSize = 0;
	this->set(orig.stack,orig.size,orig.order);
}

/*******************  FUNCTION  *********************/
Stack::~Stack ( void )
{
	if (this->stack != NULL)
		free(this->stack);
	#ifndef NDEBUG
	this->stack   = NULL;
	this->size    = 0;
	this->memSize = 0;
	#endif
}

/*******************  FUNCTION  *********************/
void Stack::set ( const Stack& orig )
{
	this->set(orig.stack,orig.size,orig.order);
}

/*******************  FUNCTION  *********************/
void Stack::set ( void** stack, int size, StackOrder order )
{
	//realloc if required
	if (this->memSize < size)
	{
		this->stack   = (void**)realloc(this->stack,size * sizeof(void**));
		this->memSize = size;
	}
	
	//copy
	if (this->order == order) 
	{
		memcpy(this->stack,stack,size * sizeof(void*));
	} else {
		for (int i = 0 ; i < size ; i++)
			this->stack[i] = stack[size - 1 - i];
	}
	
	//save size
	this->size = size;
}

/*******************  FUNCTION  *********************/
StackHash Stack::hash ( void ) const
{
	return hash(stack,size);
}

/*******************  FUNCTION  *********************/
StackHash Stack::hash ( void** stack, int size )
{
	//errors
	assert(stack != NULL);
	assert(size > 0);

	//init result
	StackHash res = 0;
	
	//trunk size to hash only the N last elements
	int hashSize = 16;
	if (size < hashSize)
		hashSize = size;

	//calc hash by doing xor on each call addresses
	for (int i = 0 ; i < hashSize ; i++)
	{
		StackHash cur = (StackHash)stack[i];
		assert(cur != 0);
		res ^= (StackHash)(cur+i+size);
		res ^= (StackHash)(cur-i+size) << 32;
	}

	return res;
}

/*******************  FUNCTION  *********************/
std::ostream& operator<< ( std::ostream& out, const Stack& obj )
{
	switch(obj.order)
	{
		case STACK_ORDER_ASC:
			for (int i = 0 ; i < obj.size ; i++)
				out << obj.stack[i] << " ";
			break;
		case STACK_ORDER_DESC:
			for (int i = obj.size - 1 ; i >= 0 ; i--)
				out << obj.stack[i] << " ";
			break;
	}
	return out;
}

/*******************  FUNCTION  *********************/
bool operator== ( const Stack& v1, const Stack& v2 )
{
	//errors
	assert(v1.stack != NULL);
	assert(v1.size > 0);
	assert(v2.stack != NULL);
	assert(v2.size > 0);
	assert(v1.order == v2.order);

	//trivial
	if (v1.size != v2.size || v1.order != v2.order)
		return false;
	
	//check content starting by
	for (int i = 0 ; i < v1.size ; i++)
		if (v1.stack[i] != v2.stack[i])
			return false;

	//ok this is good
	return true;
}

/*******************  FUNCTION  *********************/
void typeToJson ( htopml::JsonState& json, std::ostream& stream, const Stack& obj )
{
	//select running mode
	json.openArray();
	switch(obj.order)
	{
		case STACK_ORDER_ASC:
			for (int i = 0 ; i < obj.size ; i++)
			{
				char buffer[64];
				sprintf(buffer,"%p",obj.stack[i]);
				json.printValue(buffer);
			}
			break;
		case STACK_ORDER_DESC:
			for (int i = obj.size - 1 ; i >= 0 ; i--)
			{
				char buffer[64];
				sprintf(buffer,"%p",obj.stack[i]);
				json.printValue(buffer);
			}
			break;
	}
	json.closeArray();
}

/*******************  FUNCTION  *********************/
void Stack::grow ( void )
{
	//if not allocated
	if (this->stack == NULL)
	{
		this->stack = (void**)malloc(sizeof(void*) * CALL_STACK_DEFAULT_SIZE);
		this->memSize = CALL_STACK_DEFAULT_SIZE;
		this->size = 0;
	} else {
		//cal next size, double for small and add threshold if too large
		if (this->memSize <= CALL_STACK_GROW_THRESHOLD)
			this->memSize *= 2;
		else
			this->memSize += CALL_STACK_GROW_THRESHOLD;

		//resize memory
		this->stack = (void**)realloc(this->stack,this->memSize * sizeof(void*));
	}
}

/*******************  FUNCTION  *********************/
bool Stack::isValid ( void ) const
{
	return (this->stack != NULL && this->size > 0);
}

/*******************  FUNCTION  *********************/
int Stack::getSize ( void ) const
{
	return size;
}

/*******************  FUNCTION  *********************/
void Stack::resolveSymbols ( FuncNameDic& dic ) const
{
	for (int i = 0 ; i < size ; i++)
		dic.getName(stack[i]);
}

/*******************  FUNCTION  *********************/
void* Stack::getCallee(void ) const
{
	switch(order)
	{
		case STACK_ORDER_ASC:
			return stack[0];
		case STACK_ORDER_DESC:
			return stack[size-1];
	}
}

/*******************  FUNCTION  *********************/
void* Stack::getCaller(void ) const
{
	switch(order)
	{
		case STACK_ORDER_ASC:
			return stack[1];
		case STACK_ORDER_DESC:
			return stack[size-2];
	}
}

}
