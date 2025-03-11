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
	int unknownName = this->dict.getId("MALT_UNKNOWN_PYTHON");
	const PythonCallSite siteUnknown = {unknownName, unknownName, 0};
	this->siteMap[siteUnknown] = MALT_PYTHON_UNKNOWN_FUNC_ID;

	//build init map
	int initName = this->dict.getId("MALT_PYTHON_ROOT");
	const PythonCallSite siteInit = {initName, initName, 0};
	this->siteMap[siteInit] = MALT_PYTHON_INIT_FUNC_ID;

	//build null map
	int nullName = this->dict.getId("MALT_PYTHON_NULL_FRAME");
	const PythonCallSite siteNull = {nullName, nullName, 0};
	this->siteMap[siteNull] = MALT_PYTHON_NULL_FUNC_ID;

	//build C bridge map
	int cBridgeName = this->dict.getId("MALT_PYTHON_C_BRIDGE_FRAME");
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
	LangAddress addr = this->frameToLangAddress(parentFrame);
	Py_DECREF(parentFrame);
	return addr;
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
	//get fast
	//LangAddress addrFast = this->fastFrameToLangAddress(frame);

	//get slow
	LangAddress addrSlow = this->slowFrameToLangAddress(frame);

	//check equal
	//assert(addrFast.isNULL() || addrSlow == addrFast);

	//ok
	return addrSlow;
}

/**********************************************************/
LangAddress PythonSymbolTracker::slowFrameToLangAddress(PyFrameObject * frame)
{
	//convert
	TmpPythonCallSite tmpsite = frameToCallSite(frame);

	//build call site
	PythonCallSite site;
	site.file = this->dict.getId(tmpsite.site.file);
	site.function = this->dict.getId(tmpsite.site.function);
	site.line = tmpsite.site.line;

	//search entry
	auto it = this->siteMap.find(site);
	void * currentId;

	assert(tmpsite.site.function[0] != '\0');

	//is new or not
	if (it == this->siteMap.end()) {
		currentId = this->siteMap[site] = (void*)(this->nextIndex++);
	} else {
		currentId = it->second;
	}

	//free
	freeFrameToCallSite(tmpsite);

	//build address
	LangAddress addr(DOMAIN_PYTHON, (void*)currentId);

	//store
	//fprintf(stderr, "Entry : %p, %s, %s, %d\n", tmpsite.code, tmpsite.site.file, tmpsite.site.function, tmpsite.site.line);
	//this->codeToaddrMap[tmpsite.code] = addr.getAddress();

	//ok
	return addr;
}

/**********************************************************/
LangAddress PythonSymbolTracker::fastFrameToLangAddress(PyFrameObject * frame)
{
	//get code
	PyCodeObject * currentPyCode = PyFrame_GetCode(frame);
	assert(currentPyCode != NULL);

	//search in map
	const auto it = this->codeToaddrMap.find(currentPyCode);
	if (it != this->codeToaddrMap.end()) {
		return LangAddress(DOMAIN_PYTHON, (void*)(it->second));
	} else {
		return this->slowFrameToLangAddress(frame);
	}
}

/**********************************************************/
TmpPythonCallSite PythonSymbolTracker::frameToCallSite(PyFrameObject * frame)
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
	/*if (strncmp((callsite.site.function + 1), "\0", 1) != 0){
		PyObject_Free((void*) callsite.framenameObject);
	}

	if (strncmp((callsite.site.file + 1), "\0", 1) != 0){
		PyObject_Free((void*) callsite.filenameObject);	
	}*/
	Py_DECREF((void*)callsite.site.file);
	Py_DECREF((void*)callsite.site.function);
	Py_DECREF(callsite.filenameObject);
	Py_DECREF(callsite.framenameObject);
	Py_DECREF(callsite.code);
}

/**********************************************************/
void PythonSymbolTracker::registerSymbolResolution(SymbolSolver & solver) const
{
	char buffer[4096];
	for (auto & site : this->siteMap)
	{
		const char * function = this->dict.getString(site.first.function).c_str();
		const char * file = this->dict.getString(site.first.file).c_str();
		snprintf(buffer, sizeof(buffer), "py:%s", function);
		solver.registerFunctionSymbol(LangAddress(DOMAIN_PYTHON, site.second), "NONE", buffer, file, site.first.line);
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
	int notFound = this->dict.getId("NOT_FOUND");
	PythonCallSite errorSite = {notFound, notFound, -1};
	return errorSite;
}

/**********************************************************/
PythonNamedCallSite PythonSymbolTracker::getNamedCallSite(LangAddress langAddr)
{
	PythonCallSite site = this->getCallSite(langAddr);
	PythonNamedCallSite namedSite;
	namedSite.file = this->dict.getString(site.file).c_str();
	namedSite.function = this->dict.getString(site.function).c_str();
	namedSite.line = site.line;
	return namedSite;
}

}
