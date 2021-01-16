/*****************************************************
             PROJECT  : MALT
             VERSION  : 1.2.2
             DATE     : 10/2020
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
#include "StackSizeAnalyser.hpp"

/*******************  NAMESPACE  ********************/
namespace MALT
{

/*******************  FUNCTION  *********************/
/**
 * Constructor of the stack analyser.
 * It init the current call stack with function pointed by 0x1 as a fake first 
 * value.
**/
StackSizeAnalyser::StackSizeAnalyser(void)
	:largestStack(STACK_ORDER_DESC),timeProfile(MALT_PROFILED_VALUE_DEFAULT_STEPS,true)
{
	this->largestSize = 0;
	currentStack.enterFunction((void*)0x1);
}

/*******************  FUNCTION  *********************/
/**
 * To be used to solve the symbols pointed by the larger stack after analysis.
 * @param symbolResolver Reference to the symbol resolver.
**/
void StackSizeAnalyser::solveSymbols(SymbolSolver& symbolResolver) const
{
	largestStack.solveSymbols(symbolResolver);
}

/*******************  FUNCTION  *********************/
/**
 * Conver the stack size analyser to json format for the final profile file.
 * @param json Reference to the json state used to make the conversion.
 * @param value Reference to the stack size anlyser to convert.
**/
void convertToJson(htopml::JsonState& json, const StackSizeAnalyser& value)
{
	json.openStruct();
	json.printField("size",value.largestSize);
	json.printField("stack",value.largestStack);
	json.printField("mem",value.largestStackMem);
	json.printField("total",value.largestStackMem.getTotalSize());
	json.printField("timeprofiler",value.timeProfile);
	json.closeStruct();
}

}
