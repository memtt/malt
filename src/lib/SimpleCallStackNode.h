#ifndef SIMPLE_CALL_STACK_NODE_H
#define SIMPLE_CALL_STACK_NODE_H

/********************  HEADERS  *********************/
#include "CallStackInfo.h"
#include "SimpleCallStack.h"

/*********************  CLASS  **********************/
class SimpleCallStackNode
{
	public:
		SimpleCallStackNode(const SimpleCallStack & stack);
		CallStackInfo & getInfo(void) {return info;};
		SimpleCallStack & getCallStack(void) { return callStack;};
	public:
		friend std::ostream & operator << (std::ostream & out,const SimpleCallStackNode & tracer);
		friend void typeToJson(htopml::JsonState & json,std::ostream& stream, const SimpleCallStackNode & value);
	private:
		SimpleCallStack callStack;
		CallStackInfo info;
};

#endif //SIMPLE_CALL_STACK_NODE_H
