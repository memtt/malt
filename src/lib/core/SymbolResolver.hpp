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
#include <map>
#include <set>
#include <vector>
#include <ostream>
#include <cstdio>
#include <json/JsonState.h>
#include <common/STLInternalAllocator.hpp>
#include "LinuxProcMapReader.hpp"

/*******************  FUNCTION  *********************/
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
class SymbolResolver
{
	public:
		SymbolResolver(void);
		~SymbolResolver(void);
		const char * getName(void * callSite);
		void registerAddress(void * callSite);
		const char* setupNewEntry(void* callSite);
		const char * setupNewEntry(void * callSite,const std::string & name);
		void loadProcMap(void);
		void resolveNames(void);
		void resolveMaqaoNames(void);
		const CallSite * getCallSiteInfo(void * site) const;
		const std::string & getString(int id) const;
		bool isSameFuntion(const CallSite * s1,void * s2) const;
		bool procMapIsLoaded(void) const;
		void registerMaqaoFunctionSymbol(int funcId,const char * funcName,const char * file,int line);
	public:
		friend std::ostream & operator << (std::ostream & out,const SymbolResolver & dic);
		friend void convertToJson(htopml::JsonState & json, const SymbolResolver & value);
	private:
		LinuxProcMapEntry * getMapEntry(void * callSite);
		void resolveNames(LinuxProcMapEntry * procMapEntry);
		int getString(const char* value);
		void resolveMissings(void);
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
