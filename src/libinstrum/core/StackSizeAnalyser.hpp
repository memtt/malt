/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.6
*    DATE     : 03/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/core/StackSizeAnalyser.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (ECR) - 2014
*    AUTHOR   : Sébastien Valat - 2014 - 2021
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

#ifndef MALT_STACK_SIZE_ANALYSER_HPP
#define MALT_STACK_SIZE_ANALYSER_HPP

/**********************************************************/
//standard
//locals
#include "core/Trigger.hpp"
#include <stacks/Stack.hpp>
#include <valprof/ProfiledStateValue.hpp>
#include "StackSizeTracker.hpp"
#include <stacks/EnterExitStack.hpp>

/**********************************************************/
namespace MALT
{

/**********************************************************/
/**
 * @brief Class used to track the stack size and keep the largest stack seen.
**/
class StackSizeAnalyser
{
	public:
		StackSizeAnalyser(void);
		inline void onEnterFunc(LangAddress funcAddr, size_t stackPointer = 0);
		inline void onExitFunc(LangAddress funcAddr);
		void solveSymbols(SymbolSolver & symbolResolver) const;
	public:
		friend void convertToJson(htopml::JsonState& json, const StackSizeAnalyser& value);
	private:
		/** Follow current position stack and calls. **/
		EnterExitStack currentStack;
		/** Follow memory used by each call in the current stack. **/
		StackSizeTracker currentStackMem;
		/** Size of the largest stack usage seen. **/
		unsigned long largestSize;
		/** Dump of memory used by each call in the largest stack. **/
		StackSizeTracker largestStackMem;
		/** Dump of the largest call stack. **/
		Stack largestStack;
		/** Track stack size over time. **/
		ProfiledStateValue timeProfile;
		/** Trigger  */
		Trigger trigger;
};

/**********************************************************/
void maltDumpOnEvent(void);

/**********************************************************/
/**
 * Function to be used when we enter in function.
 * @param funcAddr Function pointer to the function we enter in.
 * @param stackPointer The address of the stack at the call time. This is used to
 * compute the stack. If 0, then it automatically fetch it inside.
**/
inline void StackSizeAnalyser::onEnterFunc(LangAddress funcAddr, size_t stackPointer)
{
	//update current
	currentStack.enterFunction(funcAddr);
	if (stackPointer == 0)
		currentStackMem.enter();
	else
		currentStackMem.enter(stackPointer);
	
	//get current size
	size_t cur = currentStackMem.getSize();
	
	//update time profile
	timeProfile.onUpdateValue(cur,funcAddr);
	
	//check if largest
	if (cur > largestSize)
	{
		largestSize = cur;
		largestStackMem = currentStackMem;
		largestStack = currentStack;
	}

	//need dump
	if (this->trigger.onThreadStackUpdate(cur))
		maltDumpOnEvent();
}

/**********************************************************/
/**
 * Function to be used when we exit from a function.
 * @param funcAddr Function pointer to the function we exit.
**/
inline void StackSizeAnalyser::onExitFunc(LangAddress funcAddr)
{
	//update current
	currentStack.exitFunction(funcAddr);
	currentStackMem.exit();
	
	//get current size
	size_t cur = currentStackMem.getSize();
	
	//update time profile
	timeProfile.onUpdateValue(cur,funcAddr);

	//check if largest
	if (cur > largestSize)
	{
		largestSize = cur;
		largestStackMem = currentStackMem;
		largestStack = currentStack;
	}

	//need dump
	if (this->trigger.onThreadStackUpdate(cur))
		maltDumpOnEvent();
}

}

#endif //MALT_STACK_SIZE_ANALYSER_HPP
