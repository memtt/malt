/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 02/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/core/StackSizeAnalyser.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (ECR) - 2014
*    AUTHOR   : Sébastien Valat - 2021 - 2024
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
#include "common/Options.hpp"
#include "StackSizeAnalyser.hpp"

/**********************************************************/
namespace MALT
{

/**********************************************************/
/**
 * Constructor of the stack analyser.
 * It init the current call stack with function pointed by 0x1 as a fake first 
 * value.
**/
StackSizeAnalyser::StackSizeAnalyser(void)
	:largestStack(STACK_ORDER_DESC),timeProfile(MALT_PROFILED_VALUE_DEFAULT_STEPS,true),trigger(*gblOptions)
{
	this->largestSize = 0;
	currentStack.enterFunction(LangAddress(DOMAIN_C, (void*)0x1));
}

/**********************************************************/
/**
 * To be used to solve the symbols pointed by the larger stack after analysis.
 * @param symbolResolver Reference to the symbol resolver.
**/
void StackSizeAnalyser::solveSymbols(SymbolSolver& symbolResolver) const
{
	largestStack.solveSymbols(symbolResolver);
}

/**********************************************************/
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
