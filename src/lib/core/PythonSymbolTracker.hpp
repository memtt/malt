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
#include <Python.h>
#include "common/STLInternalAllocator.hpp"
#include "SymbolSolver.hpp"
#include "stacks/LangAddress.hpp"

/**********************************************************/
namespace MALT
{

/**********************************************************/
#define MALT_PYTHON_UNKNOWN_FUNC_ID ((void*)0x1)
#define MALT_PYTHON_INIT_FUNC_ID ((void*)0x2)
#define MALT_PYTHON_NULL_FUNC_ID ((void*)0x3)

/**********************************************************/
struct PythonCallSite
{
	const char * file;
	const char * function;
	int line;
};

/**********************************************************/
struct TmpPythonCallSite
{
	PythonCallSite site;
	PyObject* filenameObject;
	PyObject* framenameObject;
};

/**********************************************************/
bool operator<(const PythonCallSite & a, const PythonCallSite & b);

/**********************************************************/
typedef std::map<PythonCallSite, void*, std::less<PythonCallSite>, STLInternalAllocator<std::pair<PythonCallSite,void*> > > PythonStrCallSiteMap;

/**********************************************************/
class PythonSymbolTracker
{
	public:
		PythonSymbolTracker(void);
		~PythonSymbolTracker(void);
		void registerSymbolResolution(SymbolSolver & solver) const;
		LangAddress frameToLangAddress(PyFrameObject * frame);
		PythonCallSite getCallSite(LangAddress langAddr);
	private:
		static TmpPythonCallSite frameToCallSite(::PyFrameObject * frame);
		static void freeFrameToCallSite(TmpPythonCallSite & callsite);
	private:
		PythonStrCallSiteMap siteMap;
		size_t nextIndex{10};
};

}

#endif //MALT_SYMBOL_TRACKER_HPP
