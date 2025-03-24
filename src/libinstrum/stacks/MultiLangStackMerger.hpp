/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 10/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/stacks/StackLoopRemover.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2015 - 2024
***********************************************************/

#ifndef MALT_STACK_MULTI_LANG_MERGER
#define MALT_STACK_MULTI_LANG_MERGER

/**********************************************************/
#include "Stack.hpp"
#include "common/RangePtr.hpp"

/**********************************************************/
namespace MALT
{

/**********************************************************/
class MultiLangStackMerger
{
	public:
		MultiLangStackMerger(void);
		void mixPythonAndCStack(Stack& outStack, const Stack & cStack, const Stack & pythonStack);
		void removePythonLib(Stack& outStack, const Stack & inStack);
	private:
		bool isPythonLibAddr(LangAddress addr) const {if (addr.getDomain() != DOMAIN_C) return false; else return this->pythonRange.contains(addr.getAddress());};
		void checkPython(void);
		RangesPtr pythonRange;
};

};

#endif //MALT_STACK_MULTI_LANG_MERGER.HPP
