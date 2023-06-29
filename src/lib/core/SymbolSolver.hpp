/*****************************************************
             PROJECT  : MALT
             VERSION  : 1.2.2
             DATE     : 06/2023
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MALT_FUNC_NAME_DIC_HPP
#define MALT_FUNC_NAME_DIC_HPP

/********************  HEADERS  *********************/
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
//internal portability
#include <portability/LinuxProcMapReader.hpp>

/*******************  FUNCTION  *********************/
/** Define a function dictionnary to store addresses and related symboles. **/
typedef std::map<void *,const char*> FuncNameDicMap;

/*******************  NAMESPACE  ********************/
namespace MALT
{

/********************  STRUCT  **********************/
/**
 * Define a call site.
 * with its source informations extracted from debug symbols.
**/
struct CallSite
{
	CallSite(LinuxProcMapEntry * mapEntry = NULL);
	/** Define the line of the call site. **/
	int line;
	/** Define the file ID (pointing the string) of the call site. **/
	int file;
	/** Define the function ID (pointing the string) of the call site. **/
	int function;
	/** Pointer to the proc map entry related to the call site. **/
	LinuxProcMapEntry * mapEntry;
};

/********************  STRUCT  **********************/
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

/*********************  TYPES  **********************/
/** Map to join a raw address to a callsite description **/
typedef std::map<void*,CallSite> CallSiteMap;
/** Map to join a raw address to a mapqao call site. It uses the internal allocator as used while running. **/
typedef std::map<void*,MaqaoSite,std::less<void*>,STLInternalAllocator<std::pair<void*,MaqaoSite> > > MaqaoSiteMap;

/*********************  CLASS  **********************/
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
		void registerAddress(void * callSite);
		const char* setupNewEntry(void* callSite);
		const char * setupNewEntry(void * callSite,const std::string & name);
		void loadProcMap(void);
		void solveNames(void);
		void solveMaqaoNames(void);
		const CallSite * getCallSiteInfo(void * site) const;
		const std::string & getString(int id) const;
		bool isSameFuntion(const CallSite * s1,void * s2) const;
		bool procMapIsLoaded(void) const;
		void registerMaqaoFunctionSymbol(int funcId,const char * funcName,const char * file,int line);
		LinuxProcMapEntry * getMapEntry(void * callSite);
		void solveAslrOffsets(void);
	public:
		friend std::ostream & operator << (std::ostream & out,const SymbolSolver & dic);
		friend void convertToJson(htopml::JsonState & json, const SymbolSolver & value);
	private:
		void solveNames(LinuxProcMapEntry * procMapEntry);
		int getString(const char* value);
		void solveMissings(void);
		static char * extractSymbolName(char * value);
		size_t extractElfVaddr(const std::string & obj) const;
		bool hasASLREnabled(void) const;
		void * getASRLOffset(void * instrAddr) const;
	private:
		/** Dictionnary of symbols. **/
		FuncNameDicMap nameMap;
		/** Linux proc map extraction. **/
		LinuxProcMap procMap;
		/** Call site map to join raw addresses to call site infos. **/
		CallSiteMap callSiteMap;
		/** Dictionnary of strings (function names and file paths). **/
		std::vector<std::string> strings;
		/** Maqo call site map. **/
		MaqaoSiteMap maqaoSites;
};

/*******************  FUNCTION  *********************/
void convertToJson(htopml::JsonState & state,const LinuxProcMapEntry & entry);
void convertToJson(htopml::JsonState & state,const CallSite & entry);

/*******************  FUNCTION  *********************/
/**
 * Convert a map to json file.
 * @param json Reference to the json state to make conversion.
 * @param iterable Reference to the map to dump.
**/
template <class T> void convertToJson(htopml::JsonState & json, const std::map<void*,T> & iterable)
{
	json.openStruct();

	for (typename std::map<void*,T>::const_iterator it = iterable.begin() ; it != iterable.end() ; ++it)
	{
		char buffer[64];
		sprintf(buffer,"%p",it->first);
		json.printField(buffer,it->second);
	}

	json.closeStruct();
}

}

#endif //MALT_FUNC_NAME_DIC_HPP
