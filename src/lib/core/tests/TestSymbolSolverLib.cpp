/********************  HEADERS  *********************/
#include <stacks/BacktraceStack.hpp>

/***************** USING NAMESPACE ******************/
using namespace MALT;

/*******************  FUNCTION  *********************/
void testCalleeLib(BacktraceStack & stack)
{
	stack.loadCurrentStack();
}
