/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.3.1
*    DATE     : 06/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/core/PythonSymbolTracker.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
#include <cassert>
#include <libgen.h>
#include <algorithm>
#include <unistd.h>
#include "PythonSymbolTracker.hpp"
#include "portability/Python.hpp"

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

	//build python untracked map
	int sitePuntrackedName = this->dict.getId("MALT_PYTHON_UNTRACKED");
	const PythonCallSite sitePuntracked = {sitePuntrackedName, sitePuntrackedName, 0};
	this->siteMap[sitePuntracked] = MALT_PYTHON_UNTRACKED_ID;

	//build python untracked map
	///@TODO move this elsewhere by creating maybe a SPECIAL domain.
	int siteCUntrackedName = this->dict.getId("MALT_C_UNTRACKED");
	const PythonCallSite siteCUntracked = {siteCUntrackedName, siteCUntrackedName, 0};
	this->siteMap[siteCUntracked] = MALT_C_UNTRACKED_ID;

	//build python import
	///@TODO move this elsewhere by creating maybe a SPECIAL domain.
	int sitePyImportName = this->dict.getId("MALT_PYTHON_HIDEN_IMPORTS");
	const PythonCallSite sitePyImport = {sitePyImportName, sitePyImportName, 0};
	this->siteMap[sitePyImport] = MALT_PYTHON_IMPORT_ID;

	//init basedir
	char buffer[8192];
	char * cwd = getcwd(buffer, sizeof(buffer));
	if (cwd == nullptr)
		MALT_FATAL("Fail to read current working directory !");
	this->baseDir = canonicalize_file_name(cwd);
}

/**********************************************************/
PythonSymbolTracker::~PythonSymbolTracker(void)
{

}

/**********************************************************/
LangAddress PythonSymbolTracker::parentFrameToLangAddress(PyFrameObject * frame)
{
	//get up
	PyFrameObject * parentFrame = MALT::PyFrame_GetBack(frame);

	//if top
	if (parentFrame == NULL)
		return LangAddress(DOMAIN_PYTHON, MALT_PYTHON_INIT_FUNC_ID);

	//ok
	LangAddress addr = this->frameToLangAddress(parentFrame);
	MALT::Py_DecRef((PyObject*)parentFrame);
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
	PyCodeObject * currentPyCode = MALT::PyFrame_GetCode(frame);
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

	currentPyCode = MALT::PyFrame_GetCode(frame);
	assert(currentPyCode != NULL);

	//Fetch the file name and frame name i.e. function name in the current PyCode
	//FIXME: Currently, this makes many allocations, maybe there's a way to avoid this
	currentFilenameObject = MALT::PyUnicode_AsEncodedString(currentPyCode->co_filename, "utf-8", "strict");
	#if PY_MAJOR_VERSION == 3 && PY_MINOR_VERSION >= 11
		currentFramenameObject = MALT::PyUnicode_AsEncodedString(currentPyCode->co_qualname, "utf-8", "strict");
	#else
		currentFramenameObject = MALT::PyUnicode_AsEncodedString(currentPyCode->co_name, "utf-8", "strict");
	#endif

	assert(currentFilenameObject != NULL);
	assert(currentFramenameObject != NULL);

	//build site
	//TODO: Look into https://docs.python.org/3/reference/datamodel.html#codeobject.co_lines
	//TODO: And this https://peps.python.org/pep-0626/
	//This should be way more performant, currently this is the major overhead
	//Intuition : Py_Addr2Line is called way too many times, can we refractor the filename, framename and line number into one call of Addr2Line ??
	TmpPythonCallSite tmpsite;
	tmpsite.site.file = MALT::PyBytes_AsString(currentFilenameObject);
	tmpsite.site.function = MALT::PyBytes_AsString(currentFramenameObject);
	tmpsite.site.line = MALT::PyFrame_GetLineNumber(frame);
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
	//MALT::Py_DecRef((PyObject*)callsite.site.file);
	//MALT::Py_DecRef((PyObject*)callsite.site.function);
	MALT::Py_DecRef((PyObject*)callsite.filenameObject);
	MALT::Py_DecRef((PyObject*)callsite.framenameObject);
	MALT::Py_DecRef((PyObject*)callsite.code);
}

/**********************************************************/
std::string PythonSymbolTracker::getModulePath(const std::string & filePath) const
{
	//var
	std::string result;
	std::string remainingPath = filePath;

	//remove .py
	if (remainingPath.substr(std::max((ssize_t)remainingPath.size() - 3, 0L),3) == ".py")
		remainingPath = remainingPath.substr(0, remainingPath.size() - 3);

	//depack until see python path
	while(remainingPath != "." && remainingPath != "/") {
		//tmp
		std::string tmp = remainingPath;

		//end
		if (this->baseDir.find(remainingPath.c_str()) != std::string::npos)
			break;
		if (this->programDir.find(remainingPath.c_str()) != std::string::npos)
			break;

		//get name
		std::string dname = basename((char*)tmp.c_str());

		//end
		if (strncmp(dname.c_str(), "python3.", 8) == 0)
			break;

		//frozen
		if (strncmp(dname.c_str(), "<frozen ", 8) == 0) {
			dname = dname.substr(8, dname.size() - 9);
		}

		//build path
		if (result.empty())
			result = dname;
		else
			result = std::string(dname) + std::string(".") + result;

		//depack
		remainingPath = dirname((char*)tmp.c_str());
	}

	return result;
}

/**********************************************************/
std::string PythonSymbolTracker::getPythonPath(const std::string & path) const
{
	//var
	std::string remainingPath = path;

	//depack until see python path
	while(remainingPath != "." && remainingPath != "/") {
		//tmp
		std::string tmp = remainingPath;

		//get name
		std::string dname = basename((char*)tmp.c_str());

		//end
		if (strncmp(dname.c_str(), "python3.", 8) == 0)
			break;

		//depack
		remainingPath = dirname((char*)tmp.c_str());
	}

	return remainingPath;
}

/**********************************************************/
std::map<std::string, bool> PythonSymbolTracker::extractorPythonPaths(void) const
{
	//search path in python install or venv or prefix
	std::map<std::string, bool> result;
	for (auto & site : this->siteMap)
	{
		const String & value = this->dict.getString(site.first.file);
		if (value[0] == '/') {
			result[this->getPythonPath(value.c_str())] = true;
		}
	}

	//ok
	return result;
}

/**********************************************************/
std::string PythonSymbolTracker::unfrozeFileName(const std::string & fname, const std::map<std::string, bool> & paths) const
{
	//check
	assert(fname.substr(0, 8) == "<frozen ");

	//extract module name
	std::string filename = fname.substr(8, fname.size() - 9);
	for (size_t i = 0 ; i < filename.size() ; i++)
		if (filename[i] == '.')
			filename[i] = '/';

	//search partent path
	for (const auto & it : paths) {
		std::string fullPath = it.first + "/" + filename + std::string(".py");
		FILE * fp = fopen(fullPath.c_str(), "r");
		if (fp != nullptr) {
			fclose(fp);
			return fullPath;
		}
	}

	//not found
	return fname;
}

/**********************************************************/
void PythonSymbolTracker::registerSymbolResolution(SymbolSolver & solver)
{
	//get python paths
	std::map<std::string, bool> pythonPaths = this->extractorPythonPaths();

	//loop on all sites to register them
	char buffer[4096];
	for (auto & site : this->siteMap)
	{
		//extract strings
		const char * function = this->dict.getString(site.first.function).c_str();
		std::string file = this->dict.getString(site.first.file).c_str();

		//handle file
		if (file.substr(0, 8) == "<frozen ") {
			file = this->unfrozeFileName(file, pythonPaths);
		}

		//prepent module path before function name
		std::string mpath = getModulePath(file);
		if (*function == '\0' || strncmp(function, "<module>", 8) == 0)
			snprintf(buffer, sizeof(buffer), "py:%s.%s", mpath.c_str(), function);
		else
			snprintf(buffer, sizeof(buffer), "py:%s.%s()", mpath.c_str(), function);

		//build address
		LangAddress address(DOMAIN_PYTHON, site.second);

		//if import address
		if (strncmp(buffer, "py:importlib.", 13) == 0 && strcmp(buffer, "py:importlib._bootstrap._ModuleLock.__init__()") != 0) {
			this->importAddresses.insert(address);
		}

		//register
		solver.registerFunctionSymbol(address, "NONE", buffer, file.c_str(), site.first.line);
	}
}

/**********************************************************/
const std::set<LangAddress> & PythonSymbolTracker::getImportAddresses(void) const
{
	return this->importAddresses;
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

/**********************************************************/
void PythonSymbolTracker::setScript(const std::string & script)
{
	//if empty exit
	if (script.empty()) {
		this->programDir = this->baseDir;
		this->scriptName = "as-shell";
		return;
	}

	//calc canonical path
	char * scriptFullPathC = realpath(script.c_str(), nullptr);

	//extract pars
	char * scriptNameC = basename(scriptFullPathC);
	this->scriptName = scriptNameC;
	char * scriptDir = dirname(scriptFullPathC);
	this->programDir = scriptDir;

	//debug
	//fprintf(stderr, "PYTHON_SCRIPT = %s\n", scriptName.c_str());
	//fprintf(stderr, "PYTHON_SDIR = %s\n", programDir.c_str());

	//free mem
	free((void*)scriptFullPathC);
}

/**********************************************************/
void PythonSymbolTracker::solveExeName(void)
{
	//get path if has some
	const std::string script = this->getPyProgramName();

	//set script
	this->setScript(script);
}

/**********************************************************/
std::string PythonSymbolTracker::getPyProgramName(void) const
{
	//get sys.argv
	PyObject * objArgv = MALT::PySys_GetObject("argv");
	assert(objArgv != nullptr);

	//get first item (sys.argv[0])
	PyObject * objArgvZero = MALT::PyList_GetItem(objArgv, 0);
	assert(objArgvZero != nullptr);

	//UTF-8 encode
	PyObject * objArgvZeroEncoded = MALT::PyUnicode_AsEncodedString(objArgvZero, "utf-8", "strict");
	assert(objArgvZeroEncoded != nullptr);

	//to string
	const char * tmp = MALT::PyBytes_AsString(objArgvZeroEncoded);
	assert(tmp != nullptr);
	std::string result = tmp;

	//dec ref
	///@todo Check no leak
	//#warning "Check no leak"
	//MALT::Py_DecRef((PyObject*)tmp);
	//MALT::Py_DecRef(objArgvZeroEncoded);
	//MALT::Py_DecRef(objArgvZero);
	//MALT::Py_DecRef(objArgv);

	//ok
	return result;
}

/**********************************************************/
const std::string & PythonSymbolTracker::getScriptName(void) const
{
	return this->scriptName;
}

}
