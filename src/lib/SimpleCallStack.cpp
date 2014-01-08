/********************  HEADERS  *********************/
#include <cstdio>
#include <cassert>
#include <cstring>
#include <execinfo.h>
#include "SimpleCallStack.h"

#define CALL_STACK_DEFAULT_SIZE 32
#define CALL_STACK_GROW_THRESHOLD 1024

/*******************  FUNCTION  *********************/
SimpleCallStack::SimpleCallStack(void)
{
	this->stack = NULL;
	this->size = 0;
	this->memSize = 0;
}

/*******************  FUNCTION  *********************/
SimpleCallStack::SimpleCallStack(void** stack, int size)
{
	this->stack = NULL;
	this->set(stack,size);
}

/*******************  FUNCTION  *********************/
SimpleCallStack::SimpleCallStack(const SimpleCallStack& orig)
{
	this->stack = NULL;
	this->set(orig.stack,orig.size);
}

/*******************  FUNCTION  *********************/
void SimpleCallStack::set(void** stack, int size)
{
	//errors
	assert(stack != NULL);
	assert(size > 0);
	
	//setup
	this->stack = (void**)realloc(this->stack,sizeof(void*)*size);
	this->size = size;
	this->memSize = size;
	memcpy(this->stack,stack,size * sizeof(void*));
}

/*******************  FUNCTION  *********************/
SimpleCallStack::~SimpleCallStack(void)
{
	if (this->stack != NULL)
		free(this->stack);
	this->stack = NULL;
	this->memSize = 0;
	this->size = 0;
}

/*******************  FUNCTION  *********************/
SimpleBacktraceHash SimpleCallStack::getSimpleHash(void ) const
{
	return getSimpleHash(stack,size);
}

/*******************  FUNCTION  *********************/
SimpleBacktraceHash SimpleCallStack::getSimpleHash(void** calls, int size)
{
	//errors
	assert(calls != NULL);
	assert(size > 0);

	//init result
	SimpleBacktraceHash res = 0;
	
	//trunk size to hash only the N last elements
	int hashSize = 4;
	if (size < hashSize)
		hashSize = size;

	//calc hash by doing xor on each call addresses
	for (int i = 0 ; i < hashSize ; i++)
	{
		assert(calls[i] != NULL);
		res ^= (SimpleBacktraceHash)calls[i];
	}

	return res;
}

/*******************  FUNCTION  *********************/
bool SimpleCallStack::equal(void** callStack, int size) const
{
	//errors
	assert(callStack != NULL);
	assert(size > 0);
	assert(this->stack != NULL);
	assert(this->size > 0);

	//trivial
	if (this->size != size)
		return false;

	//check content
	for (int i = 0 ; i < size ; i++)
		if (stack[i] != callStack[i])
			return false;

	//ok this is good
	return true;
}

/*******************  FUNCTION  *********************/
bool operator==(const SimpleCallStack& v1, const SimpleCallStack& v2)
{
	//errors
	assert(v1.isValid() && v2.isValid());
	
	return v1.equal(v2.stack,v2.size);
}

/*******************  FUNCTION  *********************/
std::ostream& operator<<(std::ostream& out, const SimpleCallStack& tracer)
{
	for (int i = 0 ; i < tracer.size ; i++)
		out << tracer.stack[i] << " ";
	return out;
}

/*******************  FUNCTION  *********************/
void SimpleCallStack::resolveSymbols(FuncNameDic& dic) const
{
	for (int i = 0 ; i < size ; i++)
		dic.getName(stack[i]);
}

/*******************  FUNCTION  *********************/
void typeToJson(htopml::JsonState& json, std::ostream& stream, const SimpleCallStack& value)
{
	json.openArray();
	for (int i = 0 ; i < value.size ; i++)
	{
		char buffer[64];
		sprintf(buffer,"%p",value.stack[i]);
		json.printValue(buffer);
	}
	json.closeArray();
}

/*******************  FUNCTION  *********************/
void typeToJson(htopml::JsonState& json, std::ostream& stream, const void * value)
{
	
}

/*******************  FUNCTION  *********************/
void SimpleCallStack::loadCurrentStack(void)
{
	//if not allocated
	if (this->stack == NULL)
	{
		this->stack = (void**)malloc(sizeof(void*) * CALL_STACK_DEFAULT_SIZE);
		this->memSize = CALL_STACK_DEFAULT_SIZE;
		this->size = 0;
	}
	
	//try to load in current buffer, if not realloc and retry
	bool retry;
	do {
		//try to load with current buffer
		int loadedSize = backtrace(this->stack,this->memSize);
		assert(loadedSize <= this->memSize);
		assert(loadedSize > 0);

		//miss some entries, need to grow the buffer
		if (loadedSize == this->memSize)
		{
			//cal next size, double for small and add threshold if too large
			if (this->memSize <= CALL_STACK_GROW_THRESHOLD)
				this->memSize *= 2;
			else
				this->memSize += CALL_STACK_GROW_THRESHOLD;

			//resize memory
			this->stack = (void**)realloc(this->stack,this->memSize * sizeof(void*));
			retry = true;
		} else {
			this->size = loadedSize;
			retry = false;
		}
	} while(retry);
}
