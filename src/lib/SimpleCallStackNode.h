#ifndef SIMPLE_CALL_STACK_NODE_H
#define SIMPLE_CALL_STACK_NODE_H

/********************  HEADERS  *********************/
#include "CallStackInfo.h"
#include "Stack.h"

/*********************  CLASS  **********************/
class SimpleCallStackNode
{
	public:
		SimpleCallStackNode(const Stack & stack);
		CallStackInfo & getInfo(void) {return info;};
		Stack & getCallStack(void) { return callStack;};
	public:
		friend std::ostream & operator << (std::ostream & out,const SimpleCallStackNode & tracer);
		friend void typeToJson(htopml::JsonState & json,std::ostream& stream, const SimpleCallStackNode & value);
	private:
		Stack callStack;
		CallStackInfo info;
};

#endif //SIMPLE_CALL_STACK_NODE_H
