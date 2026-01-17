/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 11/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/stacks/MultiLangStackMerger.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
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
		//It is loaded dyanmically, need to catch dlsym to get it :(
		//bool isPyCtypes = (entry.file.find("lib-dynload/_ctypes.cpython") == 0);
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
		gblOptions->pythonStack = STACK_MODE_NONE;
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

	//TODO: currently we don't consider python -> c -> python -> c ....
	//to handle it, we need to replace a C call to the EvalFrame function one by one.

	//loop on C stack
	size_t cur = 0;
	size_t alreadyCopyPythonStack = 0;//I don't like this "bidouille" to not take the fist function call from the python binary
	for (ssize_t i = cStack.getSize() - 1 ; i >= 0 ; i--) {
		//if encounter python
		bool isPython = this->isPythonLibAddr(cStack[i]);
		if (isPython)
			alreadyCopyPythonStack++;
		//if not already copied (this means we do not handle C => ptyhon => python ... renetrance yet)
		//and not first to keep start of the binary
		if (isPython && alreadyCopyPythonStack == 2) {
			//copy
			for (ssize_t j = pythonStack.getSize() - 1 ; j >= 0 ; j--)
				if (pythonStack[j].getDomain() == DOMAIN_PYTHON)
					outStack.stack[cur++] = pythonStack[j];
			//skip
			while (i >= 0 && this->isPythonLibAddr(cStack[i])) {
				i--;
			}
		} else if (!isPython) {
			outStack.stack[cur++] = cStack[i];
		}
	}

	outStack.size = cur;

	//debug
	/*std::cerr << "-------------------------------------------" << std::endl;
	std::cerr << "MALT: C stack  : " << cStack << std::endl;
	std::cerr << "MALT: PY stack : " << pythonStack << std::endl;
	std::cerr << "MALT: out stack : " << outStack << std::endl;*/
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
	for (ssize_t i = inStack.getSize() - 1 ; i >= 0 ; i--) {
		if (this->isPythonLibAddr(inStack[i]) == false)
			outStack.stack[cur++] = inStack[i];
	}

	outStack.size = cur;

	//debug
	/*std::cerr << "-------------------------------------------" << std::endl;
	std::cerr << "MALT: in stack  : " << inStack << std::endl;
	std::cerr << "MALT: out stack : " << outStack << std::endl;*/
}

}
