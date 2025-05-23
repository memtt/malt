/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 10/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/core/SegmentTracker.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (ECR) - 2014
*    AUTHOR   : Sébastien Valat - 2024
***********************************************************/

#ifndef MALT_SYMBOL_TRACKER_HPP
#define MALT_SYMBOL_TRACKER_HPP

/**********************************************************/
#include <map>
#include "portability/Python.hpp"
#include "common/STLInternalAllocator.hpp"
#include "SymbolSolver.hpp"
#include "stacks/LangAddress.hpp"
#include "stacks/Stack.hpp"
#include "common/StringIdDictionnary.hpp"

/**********************************************************/
namespace MALT
{

/**********************************************************/
#define MALT_PYTHON_UNKNOWN_FUNC_ID ((void*)0x1)
#define MALT_PYTHON_INIT_FUNC_ID ((void*)0x2)
#define MALT_PYTHON_NULL_FUNC_ID ((void*)0x3)
#define MALT_PYTHON_C_BRIDGE_FUNC_ID ((void*)0x4)

/**********************************************************/
struct PythonCallSite
{
	int file;
	int function;
	int line;
};

/**********************************************************/
struct PythonNamedCallSite
{
	const char * file;
	const char * function;
	int line;
};

/**********************************************************/
struct TmpPythonCallSite
{
	PythonNamedCallSite site;
	PyObject* filenameObject;
	PyObject* framenameObject;
	PyCodeObject* code;
};

/**********************************************************/
bool operator<(const PythonCallSite & a, const PythonCallSite & b);

/**********************************************************/
typedef std::map<PythonCallSite, void*, std::less<PythonCallSite>, STLInternalAllocator<std::pair<PythonCallSite,void*> > > PythonStrCallSiteMap;
typedef std::map<PyCodeObject *, void*, std::less<PyCodeObject*>, STLInternalAllocator<std::pair<PyCodeObject*,void*> > > PythonPyCodeToAddrMap;

/**********************************************************/
class PythonSymbolTracker
{
	public:
		PythonSymbolTracker(void);
		~PythonSymbolTracker(void);
		void registerSymbolResolution(SymbolSolver & solver) const;
		LangAddress parentFrameToLangAddress(PyFrameObject * frame);
		LangAddress frameToLangAddress(PyFrameObject * frame);
		PythonCallSite getCallSite(LangAddress langAddr);
		PythonNamedCallSite getNamedCallSite(LangAddress langAddr);
		void makeStackPythonDomain(Stack & stack);
	private:
		LangAddress fastFrameToLangAddress(PyFrameObject * frame);
		LangAddress slowFrameToLangAddress(PyFrameObject * frame);
		static TmpPythonCallSite frameToCallSite(PyFrameObject * frame);
		static void freeFrameToCallSite(TmpPythonCallSite & callsite);
		std::string getModulePath(const std::string & filePath) const;
		std::map<std::string, bool> extractorPythonPaths(void) const;
		std::string getPythonPath(const std::string & path) const;
		std::string unfrozeFileName(const std::string & fname, const std::map<std::string, bool> & paths) const;
	private:
		PythonStrCallSiteMap siteMap;
		size_t nextIndex{10};
		PythonPyCodeToAddrMap codeToaddrMap;
		StringIdDictionnary dict;
		std::string baseDir;
};

}

#endif //MALT_SYMBOL_TRACKER_HPP
