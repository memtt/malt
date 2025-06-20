/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.3.1
*    DATE     : 05/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/stacks/StackReducer.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
//internal
#include "StackReducer.hpp"

/**********************************************************/
namespace MALT
{

/**********************************************************/
StackReducer::StackReducer(int groupeSize)
	:tmpStack(STACK_ORDER_DESC)
{
	//assign
	this->groupeSize = groupeSize;
}

/**********************************************************/
StackReducer::~StackReducer(void)
{

}

/**********************************************************/
void StackReducer::reduce(Stack & stack)
{
	//check
	assert(this->groupeSize <= 256);
	LangAddress bufferGroup[256];
	LangAddress bufferFinal[256];

	//copy the base
	size_t offset = 0;
	size_t finalOffset = 1;
	for (offset = 0 ; stack.getSize() - offset > this->groupeSize; offset += this->groupeSize) {
		//start
		size_t start = 0;
		if (offset > 0) {
			bufferGroup[0] = bufferFinal[0];
			start = 1;
		}

		//extract base
		for (size_t i = start ; i < this->groupeSize ; i++)
			bufferGroup[i] = stack[stack.getSize() - offset - i - 1];

		//search in
		tmpStack.set(bufferGroup, this->groupeSize, STACK_ORDER_DESC);
		auto it = this->registry.find(tmpStack);

		//if found or not
		if (it != this->registry.end()) {
			bufferFinal[0] = it->second;
		} else {
			const size_t id = this->registry.size() + 1;
			bufferFinal[0] = LangAddress(DOMAIN_SUB_STACK, reinterpret_cast<void*>(id));
			this->registry[tmpStack] = LangAddress(DOMAIN_SUB_STACK, reinterpret_cast<void*>(id));
		}
	}
	
	//fill end
	while (offset < stack.getSize()) {
		assert(finalOffset < sizeof(bufferFinal));
		bufferFinal[finalOffset] = stack[stack.getSize() - 1 - offset];
		assert(stack[stack.getSize() - 1 - offset].getDomain() == DOMAIN_C);
		assert(bufferFinal[finalOffset].getDomain() == DOMAIN_C);
		finalOffset++;
		offset++;
	}

	//set again the stack
	stack.set(bufferFinal, finalOffset, STACK_ORDER_DESC);
}

std::ostream & operator<<(std::ostream & out, const StackReducer & reducer)
{
	out << "----------------- StackReducer ------------------" << std::endl;
	out << "groupSize : " << reducer.groupeSize << std::endl;
	out << "registry  : " << std::endl;
	for (const auto & it : reducer.registry)
		out << "\t" << it.second << " => " << it.first << std::endl;
	return out;
}

}
