/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 10/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/core/SimpleCallStackNode.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (ECR) - 2014
*    AUTHOR   : Sébastien Valat - 2021 - 2024
***********************************************************/

/**********************************************************/
#include <json/JsonState.h>
#include "SimpleCallStackNode.hpp"

/**********************************************************/
namespace MALT
{

/**********************************************************/
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

/**********************************************************/
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

/**********************************************************/
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
