/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 08/2025
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
#include "common/CodeTiming.hpp"

/**********************************************************/
namespace MALT
{

/**********************************************************/
/**
 * Constructor of the PythonSymbolTracker. It mostly define and register the infos
 * for the static and generic code location to assign them with an ID.
 */
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
/**
 * Destructor of the object.
 */
PythonSymbolTracker::~PythonSymbolTracker(void)
{

}

/**********************************************************/
/**
 * Convert a Python parent frame object into a LangAddress ready to be placed
 * into the MALT internal call stack representation. It first make a backtrace
 * up to get the parent, then make the translation. The link between
 * the raw addr using in LangAddress and the translation is stored into
 * this PythonSymbolTracker object.
 * @param frame The frame to translate.
 * @return A LangAddress in the Python domain.
 */
LangAddress PythonSymbolTracker::parentFrameToLangAddress(PyFrameObject * frame)
{
	//check
	assert(frame != nullptr);

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
/**
 * Convert by batch all the stack content which is cached python frame
 * for latter solving into and standard LangAddress and get ride of the
 * blocked memory by those frames.
 * @param stack The stack to convert.
 */
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
/**
 * Convert a Python frame object into a LangAddress ready to be placed
 * into the MALT internal call stack representation. The link between
 * the raw addr using in LangAddress and the translation is stored into
 * this PythonSymbolTracker object.
 * @param frame The frame to translate.
 * @return A LangAddress in the Python domain.
 */
LangAddress PythonSymbolTracker::frameToLangAddress(PyFrameObject * frame, PythonTranslationLineCache * lineCache)
{
	//get fast
	//LangAddress addrFast = this->fastFrameToLangAddress(frame);

	//skip if disabled
	#ifndef MALT_ENABLE_CACHING
		lineCache = nullptr;
	#endif //MALT_ENABLE_CACHING

	//get slow
	LangAddress addrSlow = this->slowFrameToLangAddress(frame, lineCache);

	//check
	/*#ifndef NDEBUG
		LangAddress addrSlow2 = this->slowFrameToLangAddress(frame, nullptr);
		assume(addrSlow == addrSlow2, "Cache made mistakes !");
	#endif*/

	//check equal
	//assert(addrFast.isNULL() || addrSlow == addrFast);

	//ok
	return addrSlow;
}

/**********************************************************/
/**
 * Slow approach to make the translation by making the full work of
 * extracting the strings and comparing them.
 * @param frame The frame to convert.
 * @return A LangAddress in the Python domain.
 */
LangAddress PythonSymbolTracker::slowFrameToLangAddress(PyFrameObject * frame, PythonTranslationLineCache * lineCache)
{
	//convert
	TmpPythonCallSite tmpsite = frameToCallSite(frame, lineCache);

	//if not cache, set
	PythonCallSite site;
	if (tmpsite.cached) {
		assert(lineCache != nullptr);
		site = tmpsite.cacheEntry;
	} else {
		//build call site
		assert(tmpsite.site.function[0] != '\0');
		site.file = this->dict.getId(tmpsite.site.file);
		site.function = this->dict.getId(tmpsite.site.function);
		site.line = tmpsite.site.line;
		if (lineCache != nullptr && site.line >= 0 && site.function >= 0 && site.file >= 0) {
			lineCache->set(site.line, site);
		}
	}

	//search entry
	void * currentId = nullptr;
	CODE_TIMING("pySearchInsertSite",{
		void * const * cacheResult = nullptr;
		#ifdef MALT_ENABLE_CACHING
			cacheResult = siteMapCache.get(site);
		#endif //MALT_ENABLE_CACHING
		if (cacheResult != nullptr) {
			currentId = *cacheResult;
		} else {
			auto it = this->siteMap.find(site);
			//is new or not
			if (it == this->siteMap.end()) {
				currentId = this->siteMap[site] = (void*)(this->nextIndex++);
				it = this->siteMap.find(site);
			} else {
				currentId = it->second;
			}
			#ifdef MALT_ENABLE_CACHING
				siteMapCache.set(&it->first, currentId);
			#endif //MALT_ENABLE_CACHING
		}
	});

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
/**
 * Trying a faster approach by playing with pointers to cache a value.
 * 
 * @b Note: this is not working !
 * 
 * @param frame The frame to translate.
 * @return A LangAddress in the Python domain.
 * @todo To remove.
 */
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
/**
 * Convert a frame to get a temporary python call site description. It means
 * a call site containing directly the python internal pointers not yet
 * translated to MALT strings.
 * 
 * It latter needs to be transposed to a PythonCallSite and to free the related
 * memory.
 * 
 * @param frame The frame to convert.
 * @return A TmpPythonCallSite object with the infos extracted from python about the call site.
 */
TmpPythonCallSite PythonSymbolTracker::frameToCallSite(PyFrameObject * frame, PythonTranslationLineCache * lineCache)
{
	//decl some vars
	PyCodeObject* currentPyCode = NULL;
	PyObject* currentFilenameObject = NULL;
	PyObject* currentFramenameObject = NULL;
	TmpPythonCallSite tmpsite;

	//get code
	CODE_TIMING("PyFrame_GetCode",currentPyCode = MALT::PyFrame_GetCode(frame););
	assert(currentPyCode != NULL);
	tmpsite.code = currentPyCode;

	//get line
	int line;
	CODE_TIMING("PyFrame_GetLineNumber",line = MALT::PyFrame_GetLineNumber(frame));

	//check cache
	if (lineCache != nullptr && line > -1) {
		const BacktracePythonStackTlbEntry * entry = lineCache->get(line);
		if (entry != nullptr) {
			tmpsite.cached = true;
			tmpsite.cacheEntry = *entry;
			//fprintf(stderr, "CACHE : %d, %d; %d\n", tmpsite.cacheEntry.file, tmpsite.cacheEntry.function, tmpsite.cacheEntry.line);
			return tmpsite;
		}
	}

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
	tmpsite.site.file = MALT::PyBytes_AsString(currentFilenameObject);
	tmpsite.site.function = MALT::PyBytes_AsString(currentFramenameObject);
	tmpsite.site.line = line;
	tmpsite.filenameObject = currentFilenameObject;
	tmpsite.framenameObject = currentFramenameObject;
	tmpsite.cached = false;

	//ok
	return tmpsite;
}

/**********************************************************/
/**
 * Free the memory of the call site.
 * @param callsite The temporary python call site infos coming from python.
 */
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
	if (callsite.filenameObject != nullptr)
		MALT::Py_DecRef((PyObject*)callsite.filenameObject);
	if (callsite.framenameObject != nullptr)
		MALT::Py_DecRef((PyObject*)callsite.framenameObject);
	MALT::Py_DecRef((PyObject*)callsite.code);
}

/**********************************************************/
/**
 * Extract the module path from the given filePath.
 * 
 * It is mostly required to some frozen modules to be translated so we get access to the sources
 * of to find some paths related to the launch directory.
 * 
 * @param filePath The path of the module to convert (extracted from python call site infos).
 * @return The final path as std::string.
 */
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
/**
 * Extrace the python path from the given path.
 * It walk up until seeking "python3.*".
 * @param path The path from which to extract the python path.
 * @return The computed python path.
 */
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
/**
 * Walk into the obtained paths to find the python one in order
 * to know how to build the paths to frozen modules or non full path
 * modules.
 * @return Return a map with the different python paths.
 * @todo Could be a std::set in place.
 */
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
/**
 * Convert a frozen module name into a valid module path so the GUI can
 * access the source code of this frozen python module.
 * @param fname Frozen name.
 * @param paths List of python paths in which to search the forzen module.
 * @return The unfrozed full path to the module.
 */
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
/**
 * This function should be cassed at the end of the execution to register the symbol
 * translation into the SymbolSolver to be saved into the final profile file.
 * 
 * In python this is a trivial step at this stage because we simply has to lookup in
 * the PythonSymbolTracker std::map to get access to this info in memory as it
 * has already been solved on the fly during execution.
 * 
 * @param solver The SymbolSolver to fill.
 */
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
/**
 * Return the table of import addresses to we can fold all those calls (optionally)
 * under an IMPORT symbol in the MALT profile. It avoids to bother the user
 * in the final profile with tons of allocs which are juste due to python importing
 * its modules.
 */
const std::set<LangAddress> & PythonSymbolTracker::getImportAddresses(void) const
{
	return this->importAddresses;
}

/**********************************************************/
/**
 * From a LangAddress convert the raw pointer it contains into a PythonCallSite thanks
 * the the translation std::map containing the infos.
 * @param langAddr The LangAddress to convert.
 * @return Return all the call site infos.
 */
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
/**
 * Get the callsite infos with full names instead of the default StringIds.
 * @param langAddr The address to translate back to a call site.
 * @return The call site infos composed of full MALT strings.
 */
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
/**
 * Set the Python script name in use.
 * @param script Define the path to the script in use.
 */
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
/**
 * Solve the name of the python program in order to build the correct profile file,
 * not just to see malt-python...... as the profile file name.
 */
void PythonSymbolTracker::solveExeName(void)
{
	//get path if has some
	const std::string script = this->getPyProgramName();

	//set script
	this->setScript(script);
}

/**********************************************************/
/**
 * Use python internal routines to get the python program name in run.
 * @return The name as string.
 */
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
/**
 * Return the current script name.
 * @param return The script name under the form of a std::string.
 */
const std::string & PythonSymbolTracker::getScriptName(void) const
{
	return this->scriptName;
}

/**********************************************************/
/**
 * Mark python as activ.
 */
void PythonSymbolTracker::setPythonActivity(bool activ)
{
	this->pythonIsActiv = activ;
}

/**********************************************************/
/** Print statistics at exit. */
void PythonSymbolTracker::printStats(void) const
{
	this->dict.printStats();
	this->siteMapCache.printStats("pySiteMapCache");
}

}
