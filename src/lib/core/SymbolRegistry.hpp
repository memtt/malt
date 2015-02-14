/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MATT_FUNC_NAME_DIC_HPP
#define MATT_FUNC_NAME_DIC_HPP

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
#include <allocators/STLInternalAllocator.hpp>
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
namespace MATT
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
	int binaryStringId;
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
class SymbolRegistry
{
	public:
		SymbolRegistry(void);
		~SymbolRegistry(void);
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
	public:
		friend std::ostream & operator << (std::ostream & out,const SymbolRegistry & dic);
		friend void convertToJson(htopml::JsonState & json, const SymbolRegistry & value);
	private:
		LinuxProcMapEntry * getMapEntry(void * callSite);
		void solveNames(LinuxProcMapEntry * procMapEntry);
		int getString(const char* value);
		void solveMissings(void);
		static char * extractSymbolName(char * value);
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

#endif //MATT_FUNC_NAME_DIC_HPP
