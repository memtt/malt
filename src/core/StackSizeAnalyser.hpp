/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MATT_STACK_SIZE_ANALYSER_HPP
#define MATT_STACK_SIZE_ANALYSER_HPP

/********************  HEADERS  *********************/
//standard
//locals
#include <stacks/Stack.hpp>
#include "ProfiledValue.hpp"
#include "StackSizeTracker.hpp"
#include <stacks/EnterExitStack.hpp>

/*******************  NAMESPACE  ********************/
namespace MATT
{

class StackSizeAnalyser
{
	public:
		StackSizeAnalyser(void);
		void onEnterFunc(void * funcAddr);
		void onExitFunc(void * funcAddr);
		void resolveSymbols(SymbolResolver & symbolResolver) const;
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
		ProfiledValue timeProfile;
};

}

#endif //MATT_STACK_SIZE_ANALYSER_HPP
