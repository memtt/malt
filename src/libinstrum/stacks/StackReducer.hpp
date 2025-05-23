/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 10/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/stacks/StackLoopRemover.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2015 - 2024
***********************************************************/

#ifndef MALT_STACK_REDUCER_H
#define MALT_STACK_REDUCER_H

/**********************************************************/
//internal
#include <map>
#include "Stack.hpp"
#include "EnterExitStack.hpp"

/**********************************************************/
namespace MALT
{

/**********************************************************/
class StackReducer
{
	public:
		StackReducer(int groupeSize);
		~StackReducer(void);
		void reduce(Stack & stack);
		friend std::ostream & operator<<(std::ostream & out, const StackReducer & reducer);
	private:
		int groupeSize;
		std::map<Stack, LangAddress> registry;
		Stack tmpStack;
};

}

#endif //MALT_STACK_REDUCER_H
