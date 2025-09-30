/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 08/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/core/PythonSymbolTracker.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

#ifndef MALT_SYMBOL_TRACKER_HPP
#define MALT_SYMBOL_TRACKER_HPP

/**********************************************************/
//std C++
#include <map>
//internal
#include "portability/Python.hpp"
#include "common/STLInternalAllocator.hpp"
#include "common/StringIdDictionnary.hpp"
#include "SymbolSolver.hpp"
#include "stacks/LangAddress.hpp"
#include "stacks/Stack.hpp"
//extern
#include <StaticAssoCache.hpp>

/**********************************************************/
namespace MALT
{

/**********************************************************/
/** Define an ID to use in place of the call site address to identify an unknown function in python. */
#define MALT_PYTHON_UNKNOWN_FUNC_ID ((void*)0x1)
/** Define an ID to use in place of the call site address to identify the init function in python. */
#define MALT_PYTHON_INIT_FUNC_ID ((void*)0x2)
/** Define an ID to use in place of the call site address to identify a NULL function in python. */
#define MALT_PYTHON_NULL_FUNC_ID ((void*)0x3)
/** Define an ID to use in place of the call site address to identify that python has jumped to C. */
#define MALT_PYTHON_C_BRIDGE_FUNC_ID ((void*)0x4)
/** Define an ID to use in place of the call site address to identify a python function not tracked. */
#define MALT_PYTHON_UNTRACKED_ID ((void*)0x5)
/** Define an ID to use in place of the call site address to identify a C function not tracked. */
#define MALT_C_UNTRACKED_ID ((void*)0x6)
/** Define an ID to use in place of the call site address to identify a python function called from the initial import call. */
#define MALT_PYTHON_IMPORT_ID ((void*)0x7)

/**********************************************************/
/**
 * Define a python call site which is currently identified by a fileID, functionID and line.
 * The ID needs to be passed to a StringIdDictionnary to be translated to string.
 */
struct PythonCallSite
{
	/** ID of the string containing the file path. */
	int file;
	/** ID of the string containing the function name. */
	int function;
	/** Line of the instruction referenced. */
	int line;
};

/**********************************************************/
/** 
 * Cache some solving because when looping in enter-exit mode in a function
 * We can solve faster based on the line number.
 * Cache is flushed while exiting a function.
*/
typedef PythonCallSite BacktracePythonStackTlbEntry;

/**********************************************************/
/** 
 * Cache some solving because when looping in enter-exit mode in a function
 * We can solve faster based on the line number.
 * Cache is flushed while exiting a function.
*/
typedef numaprof::StaticAssoCache<BacktracePythonStackTlbEntry,4,32> PythonTranslationLineCache;

/**********************************************************/
/**
 * Similar to PythonCallSite but by containing directly the pointers to the strings.
 */
struct PythonNamedCallSite
{
	/** Path of the source file. */
	const char * file{nullptr};
	/** Name of the running function. */
	const char * function{nullptr};
	/** Line of the instruction in the source file. */
	int line{-1};
};

/**********************************************************/
/**
 * Structure containing a temporary description of the call site
 * based on the Python internal representation of a code location.
 * 
 * As we need to free thos elements before continuing not to trash
 * the memory we need to convert them in string IDs via the StringIdDictionnary
 * to obtain a PythonCallSite which is handled by the rest of MALT.
 */
struct TmpPythonCallSite
{
	/** Definition of the site with names. */
	PythonNamedCallSite site;
	/** Python object containing the filename. */
	PyObject* filenameObject{nullptr};
	/** Python object containing the frame */
	PyObject* framenameObject{nullptr};
	/** Python object pointing to the source code associated to the frame. */
	PyCodeObject* code{nullptr};
	/** Is solved by cache */
	bool cached{false};
	/** Value of the cache */
	BacktracePythonStackTlbEntry cacheEntry;
};

/**********************************************************/
/**
 * An operator permitting to sort the PythonCallSite objects so we can use them
 * as an index in an std::map.
 */
bool operator<(const PythonCallSite & a, const PythonCallSite & b);

/**********************************************************/
/**
 * Map indexing the call sites.
 */
typedef std::map<PythonCallSite, void*, std::less<PythonCallSite>, STLInternalAllocator<std::pair<PythonCallSite,void*> > > PythonStrCallSiteMap;
/**
 * Cache of PyCodeObject to translate si we can translate them by batch and also check if the address
 * if a fixed integer ID which can be used not to down to string comparison all the time (answer is no it is not).
 * @todo This might be removed as not used anymore in current implementation.
 */
typedef std::map<PyCodeObject *, void*, std::less<PyCodeObject*>, STLInternalAllocator<std::pair<PyCodeObject*,void*> > > PythonPyCodeToAddrMap;

/**********************************************************/
/**
 * Object permitting to track the python stack rebuilding by demangling the frames to
 * obtain the call sited.
 * 
 * @todo This class is currently slow due to lots of string comparison and memory manegement
 * done in the critical path.
 */
class PythonSymbolTracker
{
	public:
		PythonSymbolTracker(void);
		~PythonSymbolTracker(void);
		void registerSymbolResolution(SymbolSolver & solver);
		LangAddress parentFrameToLangAddress(PyFrameObject * frame);
		LangAddress frameToLangAddress(PyFrameObject * frame, PythonTranslationLineCache * lineCache = nullptr);
		PythonCallSite getCallSite(LangAddress langAddr);
		PythonNamedCallSite getNamedCallSite(LangAddress langAddr);
		void makeStackPythonDomain(Stack & stack);
		void solveExeName(void);
		const std::string & getScriptName(void) const;
		void setScript(const std::string & script);
		const std::set<LangAddress> & getImportAddresses(void) const;
		void setPythonActivity(bool activ);
		bool getPythonIsActiv(void) const {return this->pythonIsActiv;};
		void printStats(void) const;
	private:
		LangAddress fastFrameToLangAddress(PyFrameObject * frame);
		LangAddress slowFrameToLangAddress(PyFrameObject * frame, PythonTranslationLineCache * lineCache = nullptr);
		static TmpPythonCallSite frameToCallSite(PyFrameObject * frame, PythonTranslationLineCache * lineCache = nullptr);
		static void freeFrameToCallSite(TmpPythonCallSite & callsite);
		std::string getModulePath(const std::string & filePath) const;
		std::map<std::string, bool> extractorPythonPaths(void) const;
		std::string getPythonPath(const std::string & path) const;
		std::string unfrozeFileName(const std::string & fname, const std::map<std::string, bool> & paths) const;
		std::string getPyProgramName(void) const;
	private:
		/**
		 * Map permitting to assign a raw pointer value to each uniq call sited
		 * seen while bactracing python. This is usefull so we can keep to pointer
		 * base stack representation from C in the rest the MALT code base.
		 */
		PythonStrCallSiteMap siteMap;
		/** Next index to assigned as a raw address to a new uniq call site. */
		size_t nextIndex{10};
		/** Convert a python code to a uniq raw pointer value. */
		PythonPyCodeToAddrMap codeToaddrMap;
		/** 
		 * String dictionnary so we handle strings as int in the rest of the code
		 * to be faster and more efficient in memory.
		 */
		StringIdDictionnary dict;
		/**
		 * Directory in which we are currently running and in which python will
		 * by default search its dependencies when not found.
		 */
		std::string baseDir;
		/**
		 * Directory of the program so we know where python will search part of
		 * its dependencies.
		 */
		std::string programDir;
		/**
		 * Name of the main script launched into python.
		 */
		std::string scriptName;
		/**
		 * List of LangAddress related to an import feature in python so we can
		 * optionnaly mark them as ignored if the related option is enabled.
		*/
		std::set<LangAddress> importAddresses;
		/** True when python is active, false by default. */
		bool pythonIsActiv{false};
};

}

#endif //MALT_SYMBOL_TRACKER_HPP
