/*****************************************************
             PROJECT  : MALT
             VERSION  : 1.2.2
             DATE     : 10/2020
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
//extern
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cassert>
//htopml json converter
#include <json/JsonState.h>
//internals from common
#include <common/SimpleAllocator.hpp>
// #include <core/SymbolSolver.hpp>
#include <common/Debug.hpp>
//locals
#include "Stack.hpp"
// #include <core/SymbolRegistry.hpp>

/********************  MACROS  **********************/
#define CALL_STACK_DEFAULT_SIZE   32
#define CALL_STACK_GROW_THRESHOLD 1024

/*******************  NAMESPACE  ********************/
namespace MALTV2
{

/*******************  FUNCTION  *********************/
/**
 * Stack constructor to init the internal states.
 * @param order Define the element ordering depending on the instrumentation mode (backtrace of enter-exit).
**/
Stack::Stack ( StackOrder order )
{
	this->order   = order;
	this->stack   = NULL;
	this->mem     = NULL;
	this->size    = 0;
	this->memSize = 0;
}

/*******************  FUNCTION  *********************/
/**
 * Import a stack from a raw C representation, typically the one obtained from the backtrace() function.
 * For the backtrace function, use STACK_ORDER_ASC ordering.
**/
Stack::Stack(void** stack, int size,StackOrder order)
{
	this->order   = order;
	this->stack   = NULL;
	this->mem     = NULL;
	this->size    = 0;
	this->memSize = 0;
	this->set(stack,size,order);
}

/*******************  FUNCTION  *********************/
/**
 * Copy constructor.
**/
Stack::Stack ( const Stack& orig )
{
	//default
	this->order   = orig.order;
	this->stack   = NULL;
	this->mem     = NULL;
	this->size    = 0;
	this->memSize = 0;
	
	//import
	this->set(orig.stack,orig.size,orig.order);
}

/*******************  FUNCTION  *********************/
/**
 * Copy a part of the stack and skip the last elements (deepest elements).
 * This function take care of the element ordering.
**/
Stack::Stack ( const Stack& orig , int skipDepth)
{
	//errors
	assert(skipDepth >= 0);
	assert(skipDepth <= orig.size);

	//default
	this->order   = orig.order;
	this->stack   = NULL;
	this->mem     = NULL;
	this->size    = 0;
	this->memSize = 0;
	
	//import
	switch (order)
	{
		case STACK_ORDER_ASC:
			this->set(orig.stack+skipDepth,orig.size-skipDepth,orig.order);
			break;
		case STACK_ORDER_DESC:
			this->set(orig.stack,orig.size-skipDepth,orig.order);
			break;
	}
	
}

/*******************  FUNCTION  *********************/
/**
 * Stack descrutor to free internal memory.
**/
Stack::~Stack ( void )
{
	if (this->mem != NULL)
		MALT_FREE(this->mem);
	#ifndef NDEBUG
	this->stack   = NULL;
	this->mem     = NULL;
	this->size    = 0;
	this->memSize = 0;
	#endif
}

/*******************  FUNCTION  *********************/
/**
 * Permit to replace the current stack content by the given one.
**/
void Stack::set ( const Stack& orig )
{
	this->set(orig.stack,orig.size,orig.order);
}

/*******************  FUNCTION  *********************/
/**
 * Permit to replace the current stack content by the given one.
 * It can be feed by the raw representation provided by backtrace().
**/
void Stack::set ( void** stack, int size, StackOrder order )
{
	//realloc if required
	if (this->memSize < size)
	{
		this->mem     = (void**)MALT_REALLOC(this->mem,size * sizeof(void**));
		this->stack   = this->mem;
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
/**
 * Compute a short hash of the stack. The returned hash take care of the ordering so
 * generate a uniq value which is valid for the two representation.
 * @param skipDepth Ignore the deepest elements.
**/
StackHash Stack::hash ( int skipDepth ) const
{
	assert(skipDepth < size);
	assert(skipDepth >= 0);
	switch(order)
	{
		case STACK_ORDER_ASC:
			return hash(stack+skipDepth,size - skipDepth,order);
		case STACK_ORDER_DESC:
			return hash(stack,size - skipDepth,order);
		default:
			MALT_FATAL("Invalid order !");
			return 0;
	}
}

/*******************  FUNCTION  *********************/
/**
 * Internal function to compute the hash.
**/
StackHash Stack::hash ( void** stack, int size ,StackOrder order)
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

	switch(order)
	{
		case STACK_ORDER_ASC:
			//calc hash by doing xor on each call addresses
			for (int i = 0 ; i < hashSize ; i++)
			{
				StackHash cur = (StackHash)stack[i];
				assert(cur != 0);
// 				res ^= (StackHash)(cur+i+size);
// 				res ^= (StackHash)(cur-i+size) << (i%32);
				res += (StackHash)(cur) << (i%32);
			}
			res*=size;
			break;
		case STACK_ORDER_DESC:
			//calc hash by doing xor on each call addresses
			for (int i = size - 1 ; i >= size - hashSize ; i--)
			{
				StackHash cur = (StackHash)stack[i];
				assert(cur != 0);
// 				res ^= (StackHash)(cur+i+size);
// 				res ^= (StackHash)(cur-i+size) << (i%32);
				res += (StackHash)(cur) << ((size-i-1)%32);
			}
			res*=size;
			break;
	};

	return res;
}

/*******************  FUNCTION  *********************/
/**
 * Operator to read stack entries. It provide a uniq ordering by checking the internal one.
 * The external representation exposed to the user is by convention the backtrace one (ASC).
**/
void* Stack::operator[](int idx) const
{
	//errors
	assert(idx >= 0);
	
	//trivial
	if (idx < 0 || idx >= size || stack == NULL)
		return NULL;

	//depend on order
	switch(order)
	{
		case STACK_ORDER_ASC:
			return stack[idx];
		case STACK_ORDER_DESC:
			return stack[size - idx - 1];
		default:
			return NULL;
	}
}

/*******************  FUNCTION  *********************/
/**
 * Print the stack content with the backtrace ordering (ASC).
**/
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
/**
 * Compare two stacks. They must have the same internal ordering representation.
 * It first do fast check on size and ordering to give a quick answer and then
 * start comparing the stack content.
**/
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
/**
 * Compare a part of the stacks. They must have the same internal representation.
 * For the outside point of view the skip parameters are given in the backtrace ordering (ASC).
 * They ignore the deepest elements.
**/
bool Stack::partialCompare(const Stack& stack1, int skip1, const Stack& stack2, int skip2)
{
	//errors
	assert(stack1.stack != NULL);
	assert(stack2.stack != NULL);
	assert(stack1.order == stack2.order);

	//compute local sizes
	int size1 = stack1.size - skip1;
	int size2 = stack2.size - skip2;
	assert(size1 > 0);
	assert(size2 > 0);

	//trivial
	if (size1 != size2 || stack1.order != stack2.order)
		return false;
	
	//localy get the pointers
	void ** s1 = stack1.stack;
	void ** s2 = stack2.stack;
	
	//skip start for ASC mode
	if (stack1.order == STACK_ORDER_ASC)
	{
		s1 += skip1;
		s2 += skip2;
	}

	//check content
	for (int i = 0 ; i < size1 ; i++)
		if (s1[i] != s2[i])
			return false;

	//ok this is good
	return true;
}

/*******************  FUNCTION  *********************/
/**
 * Manage json code generation by following the backtrace() ordering convention (ASC).
**/
void convertToJson ( htopml::JsonState& json, const Stack& obj )
{
	//select running mode
	json.openArray();
	switch(obj.order)
	{
		case STACK_ORDER_ASC:
			for (int i = 0 ; i < obj.size ; i++)
				json.printValue(obj.stack[i]);
			break;
		case STACK_ORDER_DESC:
			for (int i = obj.size - 1 ; i >= 0 ; i--)
				json.printValue(obj.stack[i]);
			break;
	}
	json.closeArray();
}

/*******************  FUNCTION  *********************/
size_t Stack::getMemSize(void ) const
{
	return memSize;
}

/*******************  FUNCTION  *********************/
/**
 * Increase the internal memory use to store the stack.
**/
void Stack::grow ( void )
{
	//if not allocated
	if (this->stack == NULL)
	{
		this->mem = (void**)MALT_MALLOC(sizeof(void*) * CALL_STACK_DEFAULT_SIZE);
		this->memSize = CALL_STACK_DEFAULT_SIZE;
		this->size = 0;
	} else {
		//cal next size, double for small and add threshold if too large
		if (this->memSize <= CALL_STACK_GROW_THRESHOLD)
			this->memSize *= 2;
		else
			this->memSize += CALL_STACK_GROW_THRESHOLD;

		//resize memory
		this->mem = (void**)MALT_REALLOC(this->mem,this->memSize * sizeof(void*));
	}
	
	//point stack on mem (no quick skip)
	this->stack = mem;
}

/*******************  FUNCTION  *********************/
/**
 * Check if the current stack if empty of not.
**/
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
/**
 * Loop on all symbols and register them into the symbol translator.
**/
void Stack::registerSymbols ( SymbolRegistry& dic ) const
{
	MALT_FATAL("Must not be called in V1 (not supported)");
// 	if (stack != NULL)
// 		for (int i = 0 ; i < size ; i++)
// 			dic.registerAddress(stack[i]);
}

/*******************  FUNCTION  *********************/
/**
 * Return the callee, the current active function when doing backtrace(). Return NULL if no stack.
**/
void* Stack::getCallee(void ) const
{
	if (stack == NULL)
	{
		return NULL;
	} else {
		assert(size >= 1);
		switch(order)
		{
			case STACK_ORDER_ASC:
				return stack[0];
			case STACK_ORDER_DESC:
				return stack[size-1];
			default:
				return NULL;
		}
	}
}

/*******************  FUNCTION  *********************/
/**
 * Return the caller of current function in call stack. Return NULL if no stack.
**/
void* Stack::getCaller(void ) const
{
	if (stack == NULL)
	{
		return NULL;
	} else {
		assert(size >= 2);
		switch(order)
		{
			case STACK_ORDER_ASC:
				return stack[1];
			case STACK_ORDER_DESC:
				return stack[size-2];
			default:
				return NULL;
		}
	}
}

/*******************  FUNCTION  *********************/
/**
 * Copy operator.
 * @param stack Define the stack to copy.
 * @return Reference to the current stack.
**/
Stack& Stack::operator=(const Stack& stack)
{
	this->set(stack);
	return *this;
}

/*******************  FUNCTION  *********************/
/**
 * Skip the last N call in the stack to ignore wrappers inside MALT. It take in account
 * the internal element ordering.
 * @param depth Define the number of call to skip.
**/
void Stack::fastSkip(int depth)
{
	assert(depth >= 0 && depth < size);
	
	switch(order)
	{
		case STACK_ORDER_ASC:
			this->size -= depth;
			this->stack+= depth;
			break;
		case STACK_ORDER_DESC:
			this->size-= depth;
			break;
	}
}

/*******************  FUNCTION  *********************/
/**
 * Ordering operator for stacks.
 * It first compare stack sizes for fast call then compare the full stack content.
**/
bool operator<(const Stack& v1, const Stack& v2)
{
	//trivial
	if (v1.size < v2.size)
		return true;
	else if (v1.size != v2.size)
		return false;
	
	
	assert(v1.order == v2.order);

	//check content starting by
	for (int i = 0 ; i < v1.size ; i++)
		if (v1.stack[i] >= v2.stack[i])
			return false;

	return true;
}

}
