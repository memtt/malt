/*****************************************************
             PROJECT  : MALT
             VERSION  : 1.2.2
             DATE     : 10/2020
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MALT_STACK_SIZE_ANALYSER_HPP
#define MALT_STACK_SIZE_ANALYSER_HPP

/********************  HEADERS  *********************/
//standard
//locals
#include <stacks/Stack.hpp>
#include <valprof/ProfiledStateValue.hpp>
#include "StackSizeTracker.hpp"
#include <stacks/EnterExitStack.hpp>

/*******************  NAMESPACE  ********************/
namespace MALT
{

/*********************  CLASS  **********************/
/**
 * @brief Class used to track the stack size and keep the largest stack seen.
**/
class StackSizeAnalyser
{
	public:
		StackSizeAnalyser(void);
		inline void onEnterFunc(void * funcAddr);
		inline void onExitFunc(void * funcAddr);
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
};

/*******************  FUNCTION  *********************/
/**
 * Function to be used when we enter in function.
 * @param funcAddr Function pointer to the function we enter in.
**/
inline void StackSizeAnalyser::onEnterFunc(void* funcAddr)
{
	//update current
	currentStack.enterFunction(funcAddr);
	currentStackMem.enter();
	
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
}

/*******************  FUNCTION  *********************/
/**
 * Function to be used when we exit from a function.
 * @param funcAddr Function pointer to the function we exit.
**/
inline void StackSizeAnalyser::onExitFunc(void* funcAddr)
{
	//update current
	currentStack.exitFunction(funcAddr);
	currentStackMem.exit();
	
	//get current size
	size_t cur = currentStackMem.getSize();
	
	//update time profile
	timeProfile.onUpdateValue(cur,funcAddr);
}

}

#endif //MALT_STACK_SIZE_ANALYSER_HPP
