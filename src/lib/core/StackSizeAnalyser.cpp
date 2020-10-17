/*****************************************************
             PROJECT  : MALT
             VERSION  : 1.2.1
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
StackSizeAnalyser::StackSizeAnalyser(void)
	:largestStack(STACK_ORDER_DESC),timeProfile(MALT_PROFILED_VALUE_DEFAULT_STEPS,true)
{
	this->largestSize = 0;
	currentStack.enterFunction((void*)0x1);
}

/*******************  FUNCTION  *********************/
void StackSizeAnalyser::solveSymbols(SymbolSolver& symbolResolver) const
{
	largestStack.solveSymbols(symbolResolver);
}

/*******************  FUNCTION  *********************/
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
