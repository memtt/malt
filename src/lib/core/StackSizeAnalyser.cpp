/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
#include "StackSizeAnalyser.hpp"

/*******************  NAMESPACE  ********************/
namespace MATT
{

/*******************  FUNCTION  *********************/
StackSizeAnalyser::StackSizeAnalyser(void)
	:largestStack(STACK_ORDER_DESC),timeProfile(MATT_PROFILED_VALUE_DEFAULT_STEPS,true)
{
	this->largestSize = 0;
	currentStack.enterFunction((void*)0x1);
}

/*******************  FUNCTION  *********************/
void StackSizeAnalyser::resolveSymbols(SymbolResolver& symbolResolver) const
{
	largestStack.resolveSymbols(symbolResolver);
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
