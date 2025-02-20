/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 10/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/core/SymbolSolver.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (ECR) - 2014
*    AUTHOR   : Sébastien Valat - 2019 - 2024
*    AUTHOR   : Sébastien Valat (ATOS) - 2019
*    AUTHOR   : Sébastien Valat (INRIA) - 2023
***********************************************************/

#ifndef MALT_FUNC_NAME_DIC_HPP
#define MALT_FUNC_NAME_DIC_HPP

/**********************************************************/
//STD C++
#include <cstdio>
//STL C++
#include <map>
#include <set>
#include <vector>
#include <ostream>
//extern deps
#include <json/JsonState.h>
//internal common
#include <common/STLInternalAllocator.hpp>
#include <common/StringIdDictionnary.hpp>
//tools
#include <tools/Addr2Line.hpp>
//internal portability
#include <portability/LinuxProcMapReader.hpp>
//Include Stack
#include <stacks/Stack.hpp>

/**********************************************************/
/** Define a function dictionnary to store addresses and related symboles. **/
typedef std::map<void *,const char*> FuncNameDicMap;

/**********************************************************/
namespace MALT
{

/**********************************************************/
/**
 * Define a call site from the MAQAO view point.
**/
struct MaqaoSite
{
	/** File of the call site as string. **/
	const char * file;
	/** Function name of the call site as string. **/
	const char * function;
	/** Line of the call site. **/
	int line;
};

/**********************************************************/
/** Map to join a raw address to a callsite description **/
typedef std::map<LangAddress,CallSite> CallSiteMap;
/** Map to join a raw address to a mapqao call site. It uses the internal allocator as used while running. **/
typedef std::map<LangAddress,MaqaoSite,std::less<LangAddress>,STLInternalAllocator<std::pair<LangAddress,MaqaoSite> > > MaqaoSiteMap;

/**********************************************************/
/**
 * This class is used to solve the symbols at the end of the execution before
 * dumping the profile file.
 * 
 * @brief Class to solve symboles.
**/
class SymbolSolver
{
	public:
		SymbolSolver(void);
		~SymbolSolver(void);
		const char * getName(void * callSite);
		void registerAddress(LangAddress callSite);
		const char* setupNewEntry(void* callSite);
		const char * setupNewEntry(void * callSite,const std::string & name);
		void loadProcMap(void);
		void solveNames(void);
		void solveMaqaoNames(void);
		const CallSite * getCallSiteInfo(LangAddress site) const;
		const std::string & getString(int id) const;
		bool isSameFuntion(const CallSite * s1,LangAddress s2) const;
		bool procMapIsLoaded(void) const;
		void registerMaqaoFunctionSymbol(int funcId,const char * funcName,const char * file,int line);
		void registerFunctionSymbol(void * addr, const char * funcName,const char * file,int line);
		void registerFunctionSymbol(LangAddress addr, const char * funcName,const char * file,int line);
		LinuxProcMapEntry * getMapEntry(LangAddress callSite);
		void solveAslrOffsets(void);
	public:
		friend std::ostream & operator << (std::ostream & out,const SymbolSolver & dic);
		friend void convertToJson(htopml::JsonState & json, const SymbolSolver & value);
	private:
		int getString(const char* value);
		void solveMissings(void);
		static char * extractSymbolName(char * value);
		size_t extractElfVaddr(const std::string & obj) const;
	private:
		/** Dictionnary of symbols. **/
		FuncNameDicMap nameMap;
		/** Linux proc map extraction. **/
		LinuxProcMap procMap;
		/** Call site map to join raw addresses to call site infos. **/
		CallSiteMap callSiteMap;
		/** Maqo call site map. **/
		MaqaoSiteMap maqaoSites;
		/** Dictionnary */
		StringIdDictionnary stringDict;
};

/**********************************************************/
void convertToJson(htopml::JsonState & state,const LinuxProcMapEntry & entry);
void convertToJson(htopml::JsonState & state,const CallSite & entry);

/**********************************************************/
/**
 * Convert a map to json file.
 * @param json Reference to the json state to make conversion.
 * @param iterable Reference to the map to dump.
**/
template <class T> void convertToJson(htopml::JsonState & json, const std::map<LangAddress,T> & iterable)
{
	json.openStruct();

	for (typename std::map<LangAddress,T>::const_iterator it = iterable.begin() ; it != iterable.end() ; ++it)
	{
		json.printField(it->first.toString().c_str(), it->second);
	}

	json.closeStruct();
}

}

#endif //MALT_FUNC_NAME_DIC_HPP
