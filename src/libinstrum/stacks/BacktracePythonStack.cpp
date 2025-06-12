/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.6
*    DATE     : 06/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/stacks/BacktracePythonStack.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
//standard
#include <cstdio>
#include <cassert>
#include <cstring>
//internals
#include <common/Debug.hpp>
#include <common/NoFreeAllocator.hpp>
//portability
#include <tools/Backtrace.hpp>
#include "tools/BacktraceGlibc.hpp"
#include "portability/Python.hpp"
//current
#include <stacks/BacktracePythonStack.hpp>

/**********************************************************/
/**
 * Limit the maximum size of the backtraces we extract to keep resonnable things.
 * @TODO Move this into the option class to support dynamic definition.
**/
#define MALT_CALL_STACK_MAX (128*1024)

/**********************************************************/
namespace MALT 
{

/**********************************************************/
/**
 * Constructor to setup the ordering mode compatible with backtrace().
**/
BacktracePythonStack::BacktracePythonStack(PythonSymbolTracker & pythonSymbolTracker)
	:Stack(STACK_ORDER_ASC)
	,pythonSymbolTracker(pythonSymbolTracker)
{
}

/**********************************************************/
/**
 * Destructor.
**/
BacktracePythonStack::~BacktracePythonStack(void)
{
}

/**********************************************************/
PyFrameObject * BacktracePythonStack::loadCurrentFrame(void)
{
	//first grow
	this->stack = this->mem;
	if (this->stack == NULL)
		this->grow();

	//If the Python interpreter is not correctly initialised, can't get the backtrace stack
	if (MALT::PyGILState_GetThisThreadState() == 0) {
		assert(this->memSize >= 1);
		if (this->memSize < 2)
			this->grow();
		this->size = 2;
		this->stack[0].set(DOMAIN_PYTHON, MALT_PYTHON_UNKNOWN_FUNC_ID);
		this->stack[1].set(DOMAIN_PYTHON, MALT_PYTHON_INIT_FUNC_ID);
		return NULL;
	}

	//Get the Python Frame
	PyFrameObject* currentFrame = MALT::PyThreadState_GetFrame(MALT::PyGILState_GetThisThreadState());
	if (currentFrame == NULL) {
		assert(this->memSize >= 1);
		if (this->memSize < 2)
			this->grow();
		this->size = 2;
		this->stack[0].set(DOMAIN_PYTHON, MALT_PYTHON_NULL_FUNC_ID);
		this->stack[1].set(DOMAIN_PYTHON, MALT_PYTHON_INIT_FUNC_ID);
	} else {
		//Py_DecRef(currentFrame);
		//init
		this->size = 0;
	}

	//ok
	return currentFrame;
}

/**********************************************************/
LangAddress BacktracePythonStack::getCurrentFrameAddr(void)
{
	//TODO need to find a way to make this clean... It fixes crash on the exit.
	if (MALT::PyGILState_GetThisThreadState() == 0) {
		return LangAddress(DOMAIN_PYTHON, MALT_PYTHON_NULL_FUNC_ID);
	}

	PyGILState_STATE gilState = MALT::PyGILState_Ensure();

	//load current frame
	PyFrameObject * currentFrame = this->loadCurrentFrame();

	//return addr
	if (currentFrame == NULL) {
		MALT::PyGILState_Release(gilState);
		return this->stack[0];
	} else {
		LangAddress res = this->pythonSymbolTracker.frameToLangAddress(currentFrame);
		MALT::Py_DecRef((PyObject*)currentFrame);
		MALT::PyGILState_Release(gilState);
		return res;
	}
}

/**********************************************************/
//In this function the code derivated from the work of intern student (UGA)
//Bastien Levasseur
/**
 * Load the current backtrace. This function will automatically grow the internal buffer
 * to adapt to the size needed to store the full call depth.
**/
void BacktracePythonStack::loadCurrentStack(void)
{
	PyGILState_STATE state = MALT::PyGILState_Ensure();

	//load current frame
	PyFrameObject * currentFrame = this->loadCurrentFrame();

	//Fetch while we are not on the top of the stack
	this->size = 0;
	while(currentFrame != NULL){
		//convert
		LangAddress langAddr = this->pythonSymbolTracker.frameToLangAddress(currentFrame);

		//register
		this->stack[this->size++] = langAddr;

		//grow if needs
		if (this->size == this->memSize)
			this->grow();

		//Go up on the stack
		PyFrameObject * oldFrame = currentFrame;
		currentFrame = MALT::PyFrame_GetBack(currentFrame);
		MALT::Py_DecRef((PyObject*)oldFrame);
	}

	//push root
	this->stack[this->size++].set(DOMAIN_PYTHON, MALT_PYTHON_INIT_FUNC_ID);
	//grow if needs
	if (this->size == this->memSize)
		this->grow();

	//GIL
	MALT::PyGILState_Release(state);
}

/**********************************************************/
bool operator<(const PythonCallSite & a, const PythonCallSite & b)
{
	if (a.line < b.line) {
		return true;
	} else if (a.line == b.line) {
		if (a.function < b.function) {
			return true;
		} else if (a.function == b.function) {
			return a.file < b.file;
		} else {
			return false;
		}
	} else {
		return false;
	}
}

}
