/*****************************************************
             PROJECT  : MALT
             VERSION  : 1.2.0-dev
             DATE     : 08/2019
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

/*********************  TYPES  **********************/
namespace htopml
{
	class JsonState;
}

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
	int line;
	int file;
	int function;
	LinuxProcMapEntry * mapEntry;
};

/********************  STRUCT  **********************/
struct MaqaoSite
{
	const char * file;
	const char * function;
	int line;
};

/*********************  TYPES  **********************/
typedef std::map<void*,CallSite> CallSiteMap;
typedef std::map<void*,MaqaoSite,std::less<void*>,STLInternalAllocator<std::pair<void*,MaqaoSite> > > MaqaoSiteMap;

/*********************  CLASS  **********************/
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
	private:
		FuncNameDicMap nameMap;
		LinuxProcMap procMap;
		CallSiteMap callSiteMap;
		std::vector<std::string> strings;
		MaqaoSiteMap maqaoSites;
};

/*******************  FUNCTION  *********************/
void convertToJson(htopml::JsonState & state,const LinuxProcMapEntry & entry);
void convertToJson(htopml::JsonState & state,const CallSite & entry);

/*******************  FUNCTION  *********************/
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
