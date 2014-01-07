/********************  HEADERS  *********************/
#include <cassert>
#include <cstring>
#include <cstdio>
#include "SimpleCallStack.h"

/*******************  FUNCTION  *********************/
SimpleCallStack::SimpleCallStack(void** callStack, int size)
{
	//errors
	assert(callStack != NULL);
	assert(size > 0);

	//setup
	this->calls = new void*[size];
	this->size = size;
	memcpy(this->calls,callStack,size * sizeof(void*));

	//default infos
	this->cnt = 0;
}

/*******************  FUNCTION  *********************/
int SimpleCallStack::getSize(void ) const
{
	return size;
}

/*******************  FUNCTION  *********************/
SimpleBacktraceHash SimpleCallStack::getSimpleHash(void ) const
{
	return getSimpleHash(calls,size);
}

/*******************  FUNCTION  *********************/
void SimpleCallStack::incrCnt(void )
{
	cnt++;
}

/*******************  FUNCTION  *********************/
SimpleBacktraceHash SimpleCallStack::getSimpleHash(void** calls, int size)
{
	//errors
	assert(calls != NULL);
	assert(size > 0);

	//init result
	SimpleBacktraceHash res = 0;

	//calc hash by doing xor on each call addresses
	for (int i = 0 ; i < size ; i++)
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

	//trivial
	if (this->size != size)
		return false;

	//check content
	for (int i = 0 ; i < size ; i++)
		if (calls[i] != callStack[i])
			return false;

	//ok this is good
	return true;
}

/*******************  FUNCTION  *********************/
std::ostream& operator<<(std::ostream& out, const SimpleCallStack& tracer)
{
	out << "[";
	for (int i = 0 ; i < tracer.size ; i++)
		out << tracer.calls[i] << " ";
	out << "]  (cnt = " << tracer.cnt << ")";
	return out;
}

/*******************  FUNCTION  *********************/
void SimpleCallStack::resolveSymbols(FuncNameDic& dic) const
{
	for (int i = 0 ; i < size ; i++)
		dic.getName(calls[i]);
}

/*******************  FUNCTION  *********************/
void typeToJson(htopml::JsonState& json, std::ostream& stream, const SimpleCallStack& value)
{
	json.openStruct();
	json.printField("cnt",value.cnt);
	json.openFieldArray("callStack");
// 	json.printFieldArray("callStack",value.calls,value.size);
	for (int i = 0 ; i < value.size ; i++)
	{
		char buffer[64];
		sprintf(buffer,"%p",value.calls[i]);
		json.printValue(buffer);
	}
	json.closeFieldArray("callStack");
	json.closeStruct();
}

/*******************  FUNCTION  *********************/
void typeToJson(htopml::JsonState& json, std::ostream& stream, const void * value)
{
	
}
