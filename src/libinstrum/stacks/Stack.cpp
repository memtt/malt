/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.6
*    DATE     : 05/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/stacks/Stack.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (ECR) - 2014
*    AUTHOR   : Sébastien Valat - 2014
*    AUTHOR   : Bastien Levasseur - 2024
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
//extern
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cassert>
//htopml json converter
#include <json/JsonState.h>
//internals from common
#include <common/SimpleAllocator.hpp>
#include <core/SymbolSolver.hpp>
#include <common/Debug.hpp>
//locals
#include "Stack.hpp"

/**********************************************************/
#define CALL_STACK_DEFAULT_SIZE   32
#define CALL_STACK_GROW_THRESHOLD 1024

/**********************************************************/
namespace MALT
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

/**********************************************************/
/**
 * Import a stack from a raw C representation, typically the one obtained from the backtrace() function.
 * For the backtrace function, use STACK_ORDER_ASC ordering.
**/
Stack::Stack(LangAddress* stack, int size,StackOrder order)
{
	this->order   = order;
	this->stack   = NULL;
	this->mem     = NULL;
	this->size    = 0;
	this->memSize = 0;
	this->set(stack,size,order);
}

/**********************************************************/
Stack::Stack(void **stack, int size, StackOrder order, DomainType domain){
	this->order   = order;
	this->stack   = NULL;
	this->mem     = NULL;
	this->size    = 0;
	this->memSize = 0;
	this->set(stack,size,order,DOMAIN_C);
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

/**********************************************************/
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

/**********************************************************/
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

/**********************************************************/
/**
 * Permit to replace the current stack content by the given one.
**/
void Stack::set ( const Stack& orig )
{
	this->set(orig.stack,orig.size,orig.order);
}

/**********************************************************/
/**
 * Permit to replace the current stack content by the given one.
 * It can be feed by the raw representation provided by backtrace().
**/
void Stack::set (LangAddress* stack, int size, StackOrder order )
{
	//realloc if required
	if (this->memSize < size)
	{
		this->mem     = (LangAddress*)MALT_REALLOC(this->mem,size * sizeof(LangAddress));
		this->stack   = this->mem;
		this->memSize = size;
		this->onGrow(this->memSize);
	}
	
	//copy
	if (this->order == order) 
	{
		memcpy(this->stack,stack,size * sizeof(LangAddress));
	} else {
		for (int i = 0 ; i < size ; i++)
			this->stack[i] = stack[size - 1 - i];
	}
	
	//save size
	this->size = size;
}

/**********************************************************/
void Stack::set (void** stack, int size, StackOrder order, DomainType domain)
{
	//realloc if required
	if (this->memSize < size)
	{
		this->mem     = (LangAddress*)MALT_REALLOC(this->mem,size * sizeof(LangAddress));
		this->stack   = this->mem;
		this->memSize = size;
		this->onGrow(this->memSize);
	}
	
	//copy
	if (this->order == order) 
	{
		for (size_t i = 0; i < size; i++)
			this->stack[i].set(domain, stack[i]);
		
	} else {
		for (int i = 0 ; i < size ; i++)
			this->stack[i].set(domain, stack[size - 1 - i]);
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
	assert(skipDepth <= size);
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

/**********************************************************/
/**
 * Internal function to compute the hash.
**/
StackHash Stack::hash (LangAddress* stack, int size ,StackOrder order)
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
				StackHash cur = (StackHash)stack[i].getAddress();
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
				StackHash cur = (StackHash)stack[i].getAddress();
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

/**********************************************************/
/**
 * Operator to read stack entries. It provide a uniq ordering by checking the internal one.
 * The external representation exposed to the user is by convention the backtrace one (ASC).
**/
LangAddress Stack::operator[](int idx) const
{
	//errors
	assert(idx >= 0);
	
	//trivial
	if (idx < 0 || idx >= size || stack == NULL)
		return nullAddr;
		
	//depend on order
	switch(order)
	{
		case STACK_ORDER_ASC:
			return stack[idx];
		case STACK_ORDER_DESC:
			return stack[size - idx - 1];
		default:
			MALT_FATAL_ARG("Undefined order on Stack : %1").arg(this).end();
			return nullAddr;
	}
}

/**********************************************************/
/**
 * Print the stack content with the backtrace ordering (ASC).
**/
std::ostream &operator<<(std::ostream &out, const Stack &obj)
{
	for (int i = 0 ; i < obj.size ; i++)
		out << obj[i] << " ";
	return out;
}

/**********************************************************/
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
	//assert(v1.order == v2.order);

	//trivial
	if (v1.size != v2.size)
		return false;
	
	//check content starting by
	for (int i = 0 ; i < v1.size ; i++)
		if (v1[i] != v2[i])
			return false;

	//ok this is good
	return true;
}

/**********************************************************/
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
	LangAddress* s1 = stack1.stack;
	LangAddress* s2 = stack2.stack;
	
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

/**********************************************************/
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
				json.printValue(obj.stack[i].toString());
			break;
		case STACK_ORDER_DESC:
			for (int i = obj.size - 1 ; i >= 0 ; i--)
				json.printValue(obj.stack[i].toString());
			break;
	}
	json.closeArray();
}

/**********************************************************/
size_t Stack::getMemSize(void ) const
{
	return memSize;
}

/**********************************************************/
/**
 * Increase the internal memory use to store the stack.
**/
void Stack::grow ( void )
{
	//if not allocated
	if (this->stack == NULL)
	{
		this->mem = (LangAddress*)MALT_MALLOC(sizeof(LangAddress/* * */) * CALL_STACK_DEFAULT_SIZE);
		this->memSize = CALL_STACK_DEFAULT_SIZE;
		this->size = 0;
		this->onGrow(this->memSize);
	} else {
		//cal next size, double for small and add threshold if too large
		if (this->memSize <= CALL_STACK_GROW_THRESHOLD)
			this->memSize *= 2;
		else
			this->memSize += CALL_STACK_GROW_THRESHOLD;

		//resize memory
		this->mem = (LangAddress*)MALT_REALLOC(this->mem,this->memSize * sizeof(LangAddress));
		this->onGrow(this->memSize);
	}
	
	//point stack on mem (no quick skip)
	this->stack = mem;
}

/**********************************************************/
void Stack::onGrow(size_t newSize)
{
}

/**********************************************************/
/**
 * Check if the current stack if empty of not.
**/
bool Stack::isValid ( void ) const
{
	return (this->stack != NULL && this->size > 0);
}

/**********************************************************/
int Stack::getSize ( void ) const
{
	return size;
}

/**********************************************************/
/**
 * Loop on all symbols and register them into the symbol translator.
**/
void Stack::solveSymbols ( SymbolSolver& dic ) const
{
	if (stack != NULL)
		for (int i = 0 ; i < size ; i++)
			dic.registerAddress(stack[i]);
}

/**********************************************************/
/**
 * Loop on all symbols and register them into the symbol translator.
**/
String Stack::toDebugString(SymbolSolver & dic) const
{
	String final_buffer;
	if (stack != NULL) {
		for (int i = 0 ; i < size ; i++) {
			const CallSite * site = dic.getCallSiteInfo(this->stack[size - i - 1]);
			char padding[4096];
			char buffer[4096];
			memset(padding, ' ', 2*i);
			padding[2*i] = '\0';
			snprintf(buffer, sizeof(buffer), "%s%s:%d\n",padding, dic.getString(site->file).c_str(), site->line);
			final_buffer += buffer;
			snprintf(buffer, sizeof(buffer), "%s%s\n",padding, dic.getString(site->function).c_str());
			final_buffer += buffer;
		}
	}
	return final_buffer;
}

/**********************************************************/
/**
 * Loop on all symbols and register them into the symbol translator.
**/
void Stack::printSymbols( SymbolSolver& dic ) const
{
	if (stack != NULL)
		for (int i = 0 ; i < size ; i++) {
			const CallSite * site = dic.getCallSiteInfo(this->stack[size - i - 1]);
			char padding[4096];
			memset(padding, ' ', 2*i);
			padding[2*i] = '\0';
			fprintf(stderr, "%s%s:%d\n",padding, dic.getString(site->file).c_str(), site->line);
			fprintf(stderr, "%s%s\n",padding, dic.getString(site->function).c_str());
		}
}

/**********************************************************/
/**
 * Return the callee, the current active function when doing backtrace(). Return NULL if no stack.
**/
LangAddress Stack::getCallee(void ) const
{
	if (stack == NULL)
	{
		return nullAddr;
	} else {
		assert(size >= 1);
		switch(order)
		{
			case STACK_ORDER_ASC:
				return stack[0];
			case STACK_ORDER_DESC:
				return stack[size-1];
			default:
				MALT_FATAL("Undefined order on a stack");
				return nullAddr;
		}
	}
}

/**********************************************************/
/**
 * Return the caller of current function in call stack. Return NULL if no stack.
**/
LangAddress Stack::getCaller(void ) const
{
	if (stack == NULL)
	{
		return nullAddr;
	} else {
		assert(size >= 2);
		switch(order)
		{
			case STACK_ORDER_ASC:
				return stack[1];
			case STACK_ORDER_DESC:
				return stack[size-2];
			default:
				return nullAddr;
		}
	}
}

/**********************************************************/
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

/**********************************************************/
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

/**********************************************************/
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
	
	
	//assert(v1.order == v2.order);

	//check content starting by
	for (int i = 0 ; i < v1.size ; i++)
		if (v1[i] < v2[i])
			return true;
		else if (!(v1[i] == v2[i]))
			return false;

	return false;
}

/**********************************************************/
Stack & Stack::operator+=(Stack & stack)
{
	//grow
	while(this->memSize < this->size + stack.size)
		this->grow();

	//add
	for (size_t i = 0 ; i < stack.size ; i++)
		this->stack[this->size++] = stack.stack[i];

	//ok
	return *this;
}

}
