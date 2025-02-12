/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 10/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/stacks/BacktraceStack.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2014 - 2024
*    AUTHOR   : Sébastien Valat (ECR) - 2014
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
#include "../tools/BacktraceGlibc.hpp"
//current
#include <stacks/BacktracePythonStack.hpp>
//Python
#include <Python.h>

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
const size_t CST_PYTHON_UNKNOWN_FUNC_ID = 0x1;
const size_t CST_PYTHON_INIT_FUNC_ID = 0x2;
const size_t CST_PYTHON_NULL_FUNC_ID = 0x3;

/**********************************************************/
/**
 * Constructor to setup the ordering mode compatible with backtrace().
**/
BacktracePythonStack::BacktracePythonStack(void)
	:Stack(STACK_ORDER_ASC)
{
	//build unknown map
	const char * unknownName = gblInternaAlloc->strdup("MALT_UNKNOWN_PYTHON");
	const PythonCallSite siteUnknown = {unknownName, unknownName, 0};
	this->siteMap[siteUnknown] = CST_PYTHON_UNKNOWN_FUNC_ID;

	//build init map
	const char * initName = gblInternaAlloc->strdup("MALT_PYTHON_ROOT");
	const PythonCallSite siteInit = {initName, initName, 0};
	this->siteMap[siteInit] = CST_PYTHON_INIT_FUNC_ID;

	//build null map
	const char * nullName = gblInternaAlloc->strdup("MALT_PYTHON_NULL_FRAME");
	const PythonCallSite siteNull = {nullName, nullName, 0};
	this->siteMap[siteNull] = CST_PYTHON_NULL_FUNC_ID;
}

/**********************************************************/
/**
 * Destructor.
**/
BacktracePythonStack::~BacktracePythonStack(void)
{
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
	//first grow
	this->stack = this->mem;
	if (this->stack == NULL)
		this->grow();

    //If the Python interpreter is not correctly initialised, can't get the backtrace stack
	if (_PyThreadState_UncheckedGet() == NULL){
		assert(this->memSize >= 1);
		if (this->memSize < 2)
			this->grow();
		this->size = 2;
		this->stack[0].set(DOMAIN_PYTHON, (void*)CST_PYTHON_UNKNOWN_FUNC_ID);
		this->stack[1].set(DOMAIN_PYTHON, (void*)CST_PYTHON_INIT_FUNC_ID);
		return;
	}

	//Get the Python Frame
	::PyFrameObject* currentFrame = PyThreadState_GetFrame(PyGILState_GetThisThreadState());
	if (currentFrame == NULL) {
		assert(this->memSize >= 1);
		if (this->memSize < 2)
			this->grow();
		this->size = 2;
		this->stack[0].set(DOMAIN_PYTHON, (void*)CST_PYTHON_NULL_FUNC_ID);
		this->stack[1].set(DOMAIN_PYTHON, (void*)CST_PYTHON_INIT_FUNC_ID);
		return;
	}

	//init
	this->size = 0;

	//Fetch while we are not on the top of the stack
	while(currentFrame != NULL){
		//decl some vars
		PyCodeObject* currentPyCode = NULL;
		PyObject* currentFilenameObject = NULL;
		PyObject* currentFramenameObject = NULL;
		char* currentFileName = NULL;
		char* currentFrameName = NULL;
		int currentLineNumber = 0;

		currentPyCode = PyFrame_GetCode(currentFrame);
		assert(currentPyCode != NULL);

		//Fetch the file name and frame name i.e. function name in the current PyCode
		//FIXME: Currently, this makes many allocations, maybe there's a way to avoid this
		currentFilenameObject = PyUnicode_AsASCIIString(currentPyCode->co_filename);
		currentFramenameObject = PyUnicode_AsASCIIString(currentPyCode->co_qualname);

		assert(currentFilenameObject != NULL);
		assert(currentFramenameObject != NULL);

		currentFileName = PyBytes_AsString(currentFilenameObject);
		currentFrameName = PyBytes_AsString(currentFramenameObject);

		//TODO: Look into https://docs.python.org/3/reference/datamodel.html#codeobject.co_lines
		//TODO: And this https://peps.python.org/pep-0626/
		//This should be way more performant, currently this is the major overhead
		//Intuition : Py_Addr2Line is called way too many times, can we refractor the filename, framename and line number into one call of Addr2Line ??
		currentLineNumber = PyFrame_GetLineNumber(currentFrame);

		//build site
		PythonCallSite site;
		site.file = currentFileName;
		site.function = currentFrameName;
		site.line = currentLineNumber;

		//search entry
		auto it = this->siteMap.find(site);
		size_t currentId;

		//is new or not
		if (it == this->siteMap.end()) {
			PythonCallSite siteNew;
			siteNew.file = gblInternaAlloc->strdup(currentFileName);
			siteNew.function = gblInternaAlloc->strdup(currentFrameName);
			siteNew.line = currentLineNumber;
			currentId = this->siteMap[siteNew] = this->nextIndex++;
		} else {
			currentId = it->second;
		}

		//Valid names with a single chararacter are not alloced, so we don't free them
		if (strncmp((currentFileName + 1), "\0", 1) != 0){
			PyObject_Free((void*) currentFilenameObject);
		}

		if (strncmp((currentFrameName + 1), "\0", 1) != 0){
			PyObject_Free((void*) currentFramenameObject);	
		}

		//register
		this->stack[this->size++].set(DOMAIN_PYTHON, (void*)currentId);

		//grow if needs
		if (this->size == this->memSize)
			this->grow();

		//Go up on the stack
		currentFrame = PyFrame_GetBack(currentFrame);
	}

	//push root
	this->stack[this->size-1].set(DOMAIN_PYTHON, (void*)CST_PYTHON_INIT_FUNC_ID);
}

/**********************************************************/
void BacktracePythonStack::registerSymbolResolution(SymbolSolver & solver) const
{
	char buffer[4096];
	for (auto & site : this->siteMap)
	{
		snprintf(buffer, sizeof(buffer), "py:%s", site.first.function);
		solver.registerFunctionSymbol(LangAddress(DOMAIN_PYTHON, (void*)site.second), buffer, site.first.file, site.first.line);
	}
}

/**********************************************************/
bool operator<(const PythonCallSite & a, const PythonCallSite & b)
{
	if (a.line < b.line) {
		return true;
	} else if (a.line == b.line) {
		int statusFunction = strcmp(a.function, b.function);
		if (statusFunction < 0) {
			return true;
		} else if (statusFunction == 0) {
			return strcmp(a.file, b.file) < 0;
		} else {
			return false;
		}
	} else {
		return false;
	}
}

}
