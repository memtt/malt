/*****************************************************
             PROJECT  : MALT
             VERSION  : 1.2.2
             DATE     : 10/2020
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
#include <json/JsonState.h>
#include "SimpleCallStackNode.hpp"

/*******************  NAMESPACE  ********************/
namespace MALT
{

/*******************  FUNCTION  *********************/
/**
 * Constructor of the simple call stack node.
 * @param stack Define the stack to be copied locally
 * @param skipDepth Define the skip parameter to cut the given stack while 
 * copying it.
**/
SimpleCallStackNode::SimpleCallStackNode(const Stack& stack,int skipDepth)
	:callStack(stack,skipDepth)
{

}

/*******************  FUNCTION  *********************/
/**
 * Stream operator to dump the call stack node.
 * @param out Define the output stream to be used.
 * @param tracer Reference to the call stack node to dump.
 * @return Reference to the output to after use.
**/
std::ostream& operator<<(std::ostream& out, const SimpleCallStackNode& tracer)
{
	out << "[ " << tracer.callStack << " ] " << tracer.info;
	return out;
}

/*******************  FUNCTION  *********************/
/**
 * Operator to convert the given call stack node into json format.
 * @param json Reference to the json state to make the conversion.
 * @param value Reference to the call stack node to convert.
**/
void convertToJson(htopml::JsonState& json, const SimpleCallStackNode& value)
{
	json.openStruct();
	json.printField("stack",value.callStack);
	json.printField("infos",value.info);
	json.closeStruct();
}

}
