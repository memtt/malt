/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.6
*    DATE     : 05/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/stacks/StackReducer.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
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
