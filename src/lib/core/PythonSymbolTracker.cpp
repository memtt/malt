/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 10/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/core/SegmentTracker.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2014 - 2024
*    AUTHOR   : Sébastien Valat (ECR) - 2014
*    AUTHOR   : Antoine Bernard (crans.org) - 2024
***********************************************************/

/**********************************************************/
#include <cassert>
#include "PythonSymbolTracker.hpp"

/**********************************************************/
namespace MALT
{

/**********************************************************/
PythonSymbolTracker::PythonSymbolTracker(void)
{
	//build unknown map
	const char * unknownName = gblInternaAlloc->strdup("MALT_UNKNOWN_PYTHON");
	const PythonCallSite siteUnknown = {unknownName, unknownName, 0};
	this->siteMap[siteUnknown] = MALT_PYTHON_UNKNOWN_FUNC_ID;

	//build init map
	const char * initName = gblInternaAlloc->strdup("MALT_PYTHON_ROOT");
	const PythonCallSite siteInit = {initName, initName, 0};
	this->siteMap[siteInit] = MALT_PYTHON_INIT_FUNC_ID;

	//build null map
	const char * nullName = gblInternaAlloc->strdup("MALT_PYTHON_NULL_FRAME");
	const PythonCallSite siteNull = {nullName, nullName, 0};
	this->siteMap[siteNull] = MALT_PYTHON_NULL_FUNC_ID;

	//build C bridge map
	const char * cBridgeName = gblInternaAlloc->strdup("MALT_PYTHON_C_BRIDGE_FRAME");
	const PythonCallSite siteCBridge = {cBridgeName, cBridgeName, 0};
	this->siteMap[siteCBridge] = MALT_PYTHON_C_BRIDGE_FUNC_ID;
}

/**********************************************************/
PythonSymbolTracker::~PythonSymbolTracker(void)
{

}

/**********************************************************/
LangAddress PythonSymbolTracker::parentFrameToLangAddress(PyFrameObject * frame)
{
	//get up
	PyFrameObject * parentFrame = PyFrame_GetBack(frame);

	//if top
	if (parentFrame == NULL)
		return LangAddress(DOMAIN_PYTHON, MALT_PYTHON_INIT_FUNC_ID);

	//ok
	return this->frameToLangAddress(parentFrame);
}

/**********************************************************/
void PythonSymbolTracker::makeStackPythonDomain(Stack & stack)
{
	for (size_t i = 0 ; i < stack.getSize() ; i++) {
		if (stack[i].getDomain() == DOMAIN_PYTHON_FRAME) {
			PyFrameObject * frame = (PyFrameObject *)stack[i].getAddress();
			stack[i] = this->frameToLangAddress(frame);
		}
	}
}

/**********************************************************/
LangAddress PythonSymbolTracker::frameToLangAddress(PyFrameObject * frame)
{
	//convert
	TmpPythonCallSite tmpsite = frameToCallSite(frame);

	//search entry
	auto it = this->siteMap.find(tmpsite.site);
	void * currentId;

	assert(tmpsite.site.function[0] != '\0');

	//is new or not
	if (it == this->siteMap.end()) {
		PythonCallSite siteNew;
		siteNew.file = gblInternaAlloc->strdup(tmpsite.site.file);
		siteNew.function = gblInternaAlloc->strdup(tmpsite.site.function);
		siteNew.line = tmpsite.site.line;
		currentId = this->siteMap[siteNew] = (void*)(this->nextIndex++);
	} else {
		currentId = it->second;
	}

	//free
	freeFrameToCallSite(tmpsite);

	//ok
	return LangAddress(DOMAIN_PYTHON, (void*)currentId);
}

/**********************************************************/
TmpPythonCallSite PythonSymbolTracker::frameToCallSite(::PyFrameObject * frame)
{
	//decl some vars
	PyCodeObject* currentPyCode = NULL;
	PyObject* currentFilenameObject = NULL;
	PyObject* currentFramenameObject = NULL;
	char* currentFileName = NULL;
	char* currentFrameName = NULL;
	int currentLineNumber = 0;

	currentPyCode = PyFrame_GetCode(frame);
	assert(currentPyCode != NULL);

	//Fetch the file name and frame name i.e. function name in the current PyCode
	//FIXME: Currently, this makes many allocations, maybe there's a way to avoid this
	currentFilenameObject = PyUnicode_AsASCIIString(currentPyCode->co_filename);
	currentFramenameObject = PyUnicode_AsASCIIString(currentPyCode->co_qualname);

	assert(currentFilenameObject != NULL);
	assert(currentFramenameObject != NULL);

	//build site
	//TODO: Look into https://docs.python.org/3/reference/datamodel.html#codeobject.co_lines
	//TODO: And this https://peps.python.org/pep-0626/
	//This should be way more performant, currently this is the major overhead
	//Intuition : Py_Addr2Line is called way too many times, can we refractor the filename, framename and line number into one call of Addr2Line ??
	TmpPythonCallSite tmpsite;
	tmpsite.site.file = PyBytes_AsString(currentFilenameObject);
	tmpsite.site.function = PyBytes_AsString(currentFramenameObject);
	tmpsite.site.line = PyFrame_GetLineNumber(frame);
	tmpsite.filenameObject = currentFilenameObject;
	tmpsite.framenameObject = currentFramenameObject;
	tmpsite.code = currentPyCode;

	//ok
	return tmpsite;
}

/**********************************************************/
void PythonSymbolTracker::freeFrameToCallSite(TmpPythonCallSite & callsite)
{
	//Valid names with a single chararacter are not alloced, so we don't free them
	if (strncmp((callsite.site.function + 1), "\0", 1) != 0){
		PyObject_Free((void*) callsite.framenameObject);
	}

	if (strncmp((callsite.site.file + 1), "\0", 1) != 0){
		PyObject_Free((void*) callsite.filenameObject);	
	}
	/*Py_DECREF(callsite.filenameObject);
	Py_DECREF(callsite.framenameObject);
	Py_DECREF(callsite.code);*/
}

/**********************************************************/
void PythonSymbolTracker::registerSymbolResolution(SymbolSolver & solver) const
{
	char buffer[4096];
	for (auto & site : this->siteMap)
	{
		snprintf(buffer, sizeof(buffer), "py:%s", site.first.function);
		solver.registerFunctionSymbol(LangAddress(DOMAIN_PYTHON, site.second), buffer, site.first.file, site.first.line);
	}
}

/**********************************************************/
PythonCallSite PythonSymbolTracker::getCallSite(LangAddress langAddr)
{
	//check
	assert(langAddr.getDomain() == DOMAIN_PYTHON);

	//search (inefficient)
	for (auto & it : this->siteMap)
		if (it.second == langAddr.getAddress())
			return it.first;

	//not found
	MALT_FATAL_ARG("Python address not found in symbole tracker : %1").arg(langAddr).end();

	//not found
	PythonCallSite errorSite = {"NOT_FOUND", "NOT_FOUND", -1};
	return errorSite;
}

}
