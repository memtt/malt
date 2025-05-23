/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 10/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/stacks/StackLoopRemover.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2015 - 2024
***********************************************************/

/**********************************************************/
#include "common/Options.hpp"
#include "portability/LinuxProcMapReader.hpp"
#include "MultiLangStackMerger.hpp"

/**********************************************************/
namespace MALT
{

/**********************************************************/
MultiLangStackMerger::MultiLangStackMerger(void)
{
	this->checkPython();
}

/**********************************************************/
void MultiLangStackMerger::checkPython(void)
{
	//search python SO lib
	LinuxProcMapReader procMapReader;
	procMapReader.load();
	bool hasOne = false;
	for (auto & entry : procMapReader) {
		//fprintf(stderr, "MALT: entry : %s\n", entry.file.c_str());
		std::string fname = basename(entry.file.c_str());
		bool islib = (fname.find("libpython3") == 0 && entry.file.find(".so") != std::string::npos);
		bool isexe = (fname.find("python3") == 0);
		if (islib || isexe) {
			//fprintf(stderr, "MALT: Python lib is in %s [%p , %p]\n", fname.c_str(), entry.lower, entry.upper);
			this->pythonRange.add(entry.lower, entry.upper);
			hasOne = true;
		}
	}

	//no python, disable python stacking
	if (hasOne == false) {
		//fprintf(stderr, "MALT: No python detected, skip python analysis");
		gblOptions->pythonMix = false;
		gblOptions->pythonStackEnum = STACK_MODE_NONE;
	}
}

/**********************************************************/
void MultiLangStackMerger::mixPythonAndCStack(Stack& outStack, const Stack & cStack, const Stack & pythonStack)
{
	//grow
	const size_t maxSize = cStack.getMemSize() + pythonStack.getMemSize();
	while (outStack.getMemSize() < maxSize)
		outStack.grow();

	//reset
	outStack.size = 0;

	//loop on C stack
	size_t cur = 0;
	bool alreadyCopyPythonStack = false;
	for (size_t i = 0 ; i < cStack.getSize() ; i++) {
		//if encounter python
		if (this->isPythonLibAddr(cStack[i]) && !alreadyCopyPythonStack) {
			//copy
			for (size_t j = 0 ; j < pythonStack.getSize() ; j++)
				if (pythonStack[j].getDomain() == DOMAIN_PYTHON)
					outStack.stack[cur++] = pythonStack[j];
			//skip
			while (i < cStack.getSize() && this->isPythonLibAddr(cStack[i]))
				i++;

			//already copied (this means we do not handle C => ptyhon => python ... renetrance yet)
			alreadyCopyPythonStack = true;
		} else {
			outStack.stack[cur++] = cStack[i];
		}
	}

	outStack.size = cur;

	//debug
	/*std::cerr << "-------------------------------------------" << std::endl;
	std::cerr << "MALT : C stack  : " << cStack << std::endl;
	std::cerr << "MALT : PY stack : " << pythonStack << std::endl;
	std::cerr << "MALT : out stack : " << outStack << std::endl;*/
}

/**********************************************************/
void MultiLangStackMerger::removePythonLib(Stack& outStack, const Stack & inStack)
{
	//grow
	while (outStack.getMemSize() < inStack.getMemSize())
		outStack.grow();

	//reset
	outStack.size = 0;

	//copy only non python addresses
	size_t cur = 0;
	for (size_t i = 0 ; i < inStack.getSize() ; i++) {
		if (this->isPythonLibAddr(inStack[i]) == false)
			outStack.stack[cur++] = inStack[i];
	}

	outStack.size = cur;

	//debug
	/*std::cerr << "-------------------------------------------" << std::endl;
	std::cerr << "MALT : in stack  : " << inStack << std::endl;
	std::cerr << "MALT : out stack : " << outStack << std::endl;*/
}

}
