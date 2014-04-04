/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
#include <cassert>
#include <dlfcn.h>
#include <cstdlib>
#include <execinfo.h>
#include <cstring>
#include <cstdio>
#include <iostream>
#include "SymbolResolver.hpp"
#include <json/JsonState.h>
#include <common/Debug.hpp>
#include <common/Array.hpp>

/*******************  NAMESPACE  ********************/
namespace MATT
{

/*******************  FUNCTION  *********************/
SymbolResolver::SymbolResolver(void )
{
	strings.push_back("??");
}

/*******************  FUNCTION  *********************/
SymbolResolver::~SymbolResolver(void )
{
	for (FuncNameDicMap::const_iterator it = nameMap.begin() ; it != nameMap.end() ; ++it)
		free((void*)it->second);
}

/*******************  FUNCTION  *********************/
const char* SymbolResolver::getName(void* callSite)
{
	//errors
	assert(callSite != NULL);

	//search in map
	FuncNameDicMap::const_iterator it = this->nameMap.find(callSite);

	//if not found, build new entry
	if (it == nameMap.end())
	{
		return setupNewEntry(callSite);
	} else {
		return it->second;
	}
}

/*******************  FUNCTION  *********************/
void SymbolResolver::registerAddress(void* callSite)
{
	//check if present, if true, nothing to do
	CallSiteMap::const_iterator it = callSiteMap.find(callSite);
	if (it != callSiteMap.end())
		return;

	//search procmap entry
	LinuxProcMapEntry * procMapEntry = getMapEntry(callSite);

	//insert
	callSiteMap[callSite].mapEntry = procMapEntry;
}

/*******************  FUNCTION  *********************/
const char* SymbolResolver::setupNewEntry(void* callSite)
{
	//errors
	assert(callSite != NULL);

	//use backtrace_symbol to extract name and line
	char ** tmp = backtrace_symbols(&callSite,1);
	//TODO move to assume
	assert(tmp != NULL);
	assert(tmp[0] != NULL);

	//copy the intersting part
	char * res = strdup(tmp[0]);
	nameMap[callSite] = res;
	free(tmp);

	//ok return
	return res;
}

/*******************  FUNCTION  *********************/
std::ostream& operator<<(std::ostream& out, const SymbolResolver& dic)
{
	for (FuncNameDicMap::const_iterator it = dic.nameMap.begin() ; it != dic.nameMap.end() ; ++it)
		out << it->first << " " << it->second << std::endl;
	return out;
}

/*******************  FUNCTION  *********************/
void convertToJson(htopml::JsonState& json, const SymbolResolver& value)
{
	json.openStruct();

// 	json.printField("entries",value.nameMap);
	
	json.printField("map",value.procMap);
	json.printField("strings",value.strings);
	json.printField("instr",value.callSiteMap);

	json.closeStruct();
}

/*******************  FUNCTION  *********************/
const char* SymbolResolver::setupNewEntry(void* callSite, const std::string& name)
{
	//search in map
	FuncNameDicMap::const_iterator it = this->nameMap.find(callSite);

	//if not found, build new entry
	if (it == nameMap.end())
	{
		char * tmp = strdup(name.c_str());
		nameMap[callSite] = tmp;
		return tmp;
	} else {
		return it->second;
	}
}

/*******************  FUNCTION  *********************/
void SymbolResolver::loadProcMap(void)
{
	//errors
	assert(procMap.empty());
	
	//open proc map
	FILE * fp = fopen("/proc/self/maps","r");
	assumeArg(fp != NULL,"Failed to read segment mapping in %1 : %2.").arg("/proc/self/map").argStrErrno().end();
	
	//loop on entries
	char buffer[4096];
	char ignored[4096];
	char fileName[4096];
	size_t ignored2;
	LinuxProcMapEntry entry;

	//loop on lines
	while (!feof(fp))
	{
		//load buffer
		char * res = fgets(buffer,sizeof(buffer),fp);
		
		//if ok, parse line
		if (res == buffer)
		{
			//parse
			int cnt = sscanf(buffer,"%p-%p %s %p %s %lu %s\n",&(entry.lower),&(entry.upper),ignored,&(entry.offset),ignored,&ignored2,fileName);
			
			//check args
			if (cnt == 7)
				entry.file = fileName;
			else if (cnt == 6)
				entry.file.clear();
			else
				MATT_FATAL_ARG("Invalid readline of proc/map entry : %1.").arg(buffer).end();
			
			//ok push
			procMap.push_back(entry);
		}
	}
	
	//close
	fclose(fp);
}

/*******************  FUNCTION  *********************/
void convertToJson(htopml::JsonState& json, const LinuxProcMapEntry& value)
{
	json.openStruct();
	json.printField("lower",value.lower);
	json.printField("upper",value.upper);
	json.printField("offset",value.offset);
	json.printField("file",value.file);
	json.closeStruct();
}

/*******************  FUNCTION  *********************/
CallSite::CallSite(LinuxProcMapEntry* mapEntry)
{
	this->mapEntry = mapEntry;
	this->line = 0;
	this->function = -1;
	this->file = -1;
}

/*******************  FUNCTION  *********************/
LinuxProcMapEntry* SymbolResolver::getMapEntry(void* callSite)
{
	//search in map by checking intervals
	for (LinuxProcMap::iterator it = procMap.begin() ; it != procMap.end() ; ++it)
		if (callSite >= it->lower && callSite < it->upper)
			return &(*it);

	//check errors
	MATT_WARNING_ARG("Caution, call site is not found in procMap : %1.").arg(callSite).end();
	return NULL;
}

/*******************  FUNCTION  *********************/
void SymbolResolver::resolveNames(void)
{
	fprintf(stderr,"Resolving symbols with addr2line...\n");
	
	//avoid to LD_PRELOAD otherwise we will create fork bomb
	setenv("LD_PRELOAD","",1);
	
	//loop on assemblies to extract names
	for (LinuxProcMap::iterator it = procMap.begin() ; it != procMap.end() ; ++it)
	{
		if (!(it->file.empty() || it->file[0] == '['))
			resolveNames(&(*it));
	}
	
	//final check for not found
	resolveMissings();
}

/*******************  FUNCTION  *********************/
/*
 * Some links :
 * man proc & man addr2line
 * http://stackoverflow.com/a/7557756/257568
 * ​http://libglim.googlecode.com/svn/trunk/exception.hpp
 * ​http://stackoverflow.com/questions/10452847/backtrace-function-inside-shared-libraries 
*/
void SymbolResolver::resolveNames(LinuxProcMapEntry * procMapEntry)
{
	//prepare command
	bool hasEntries = false;
	std::stringstream addr2lineCmd;
	addr2lineCmd << "addr2line -C -f -e " << procMapEntry->file;
	std::vector<CallSite*> lst;
	bool isSharedLib = false;
	
	//check if shared lib or exe
	if (procMapEntry->file.substr(procMapEntry->file.size()-3) == ".so")
		isSharedLib = true;
	
	//preate addr2line args
	for (CallSiteMap::iterator it = callSiteMap.begin() ; it != callSiteMap.end() ; ++it)
	{
		if (it->second.mapEntry == procMapEntry)
		{
			hasEntries = true;
			if (isSharedLib)
				addr2lineCmd << ' '  << (void*)((size_t)it->first - (size_t)procMapEntry->lower);
			else
				addr2lineCmd << ' '  << (void*)((size_t)it->first);
			lst.push_back(&it->second);
		}
	}
	
	//if no extry, exit
	if (!hasEntries)
		return;
	
	//run command
	//std::cerr << addr2lineCmd.str() << std::endl;
	FILE * fp = popen(addr2lineCmd.str().c_str(),"r");
	
	//check error, skip resolution
	if (fp == NULL)
	{
		MATT_ERROR_ARG("Fail to use addr2line on %1 to load symbols : %2.").arg(procMapEntry->file).argStrErrno().end();
		return;
	}
	
	//read all entries
	char bufferFunc[2*4096];
	char bufferFile[4096];
	int i = 0;
	while (!feof(fp))
	{
		//read the two lines
		char * funcRes = fgets(bufferFunc,sizeof(bufferFunc),fp);
		char * fileRes = fgets(bufferFile,sizeof(bufferFile),fp);
		
		if (funcRes != bufferFunc || fileRes != bufferFile)
			break;

		//std::cerr << bufferFunc;
		//std::cerr << bufferFile;

		//check end of line and remove it
		int endLine = strlen(bufferFunc);
		assumeArg(bufferFunc[endLine-1] == '\n',"Missing \\n at end of line for the function or symbol name read from addr2line : %1.").arg(bufferFile).end();
		bufferFunc[endLine-1] = '\0';

		//check errors
		assume(i < lst.size(),"Overpass lst size.");

		//search ':' separator at end of "file:line" string
		char * sep = strrchr(bufferFile,':');
		if (sep == NULL)
		{
			MATT_WARNING_ARG("Fail to split source location on ':' : %1").arg(bufferFile).end();
		} else {
			*sep='\0';
			
			//extract line
			lst[i]->line = atoi(sep+1);
			
			//get filename and function name address
			lst[i]->file = getString(bufferFile);
			//if (strcmp(bufferFunc,"??") == 0)
			//	lst[i]->function = -1;
			//else
			lst[i]->function = getString(bufferFunc);
		}

		//move next
		i++;
		//std::cerr<< std::endl;
	}
	
	//close
	int res = pclose(fp);
	if (res != 0)
	{
		MATT_ERROR_ARG("Get error while using addr2line on %1 to load symbols : %2.").arg(procMapEntry->file).argStrErrno().end();
		return;
	}

	//error
	assumeArg(i == lst.size(),"Some entries are missing from addr2line, get %1, but expect %2. (%3)").arg(i).arg(lst.size()).arg(procMapEntry->file).end();
}

/*******************  FUNCTION  *********************/
int SymbolResolver::getString(const char * value)
{
	int id = 0;
	for (std::vector<std::string>::iterator it = strings.begin() ; it != strings.end() ; ++it)
	{
		if (*it == value)
			return id;
		id++;
	}
	strings.push_back(value);
	return strings.size()-1;
}

/*******************  FUNCTION  *********************/
void convertToJson(htopml::JsonState& json, const CallSite& value)
{
	if (value.mapEntry != NULL || value.line != 0 || value.function != -1 || value.file != -1)
	{
		json.openStruct();
		if (value.file != -1)// && *value.file != "??")
			json.printField("file",value.file);
		if (value.function != -1)// && *value.function != "??")
			json.printField("function",value.function);
		if (value.line != 0)
			json.printField("line",value.line);
		if (value.mapEntry != NULL)
			json.printField("binary",value.mapEntry->file);
		json.closeStruct();
	}
}

/*******************  FUNCTION  *********************/
const CallSite* SymbolResolver::getCallSiteInfo(void* site) const
{
	CallSiteMap::const_iterator it = callSiteMap.find(site);
	if (it == callSiteMap.end())
		return NULL;
	else
		return &it->second;
}

/*******************  FUNCTION  *********************/
const std::string& SymbolResolver::getString(int id) const
{
	assert(id < strings.size());
	return strings[id];
}

/*******************  FUNCTION  *********************/
/**
 * Function to use after loading symbols with resolveNames(). It search all '??' values
 * and try a resolution of symbol names with backtrace_symbols() from glibc.
**/
void SymbolResolver::resolveMissings(void)
{
	//store symbols not resolved
	Array<void*> toResolve(16,1024,false);

	//search
	for (CallSiteMap::const_iterator it = callSiteMap.begin() ; it != callSiteMap.end() ; ++it)
		if (it->second.function == -1 || getString(it->second.function) == "??")
			toResolve.push_back(it->first);
		
	//nothing to do
	if (toResolve.size() == 0)
		return;
	
	char ** res = backtrace_symbols(toResolve.getBuffer(),toResolve.size());
	if (res != NULL)
	{
		int i = 0;
		for (CallSiteMap::iterator it = callSiteMap.begin() ; it != callSiteMap.end() ; ++it)
			if (it->second.function == -1 || getString(it->second.function) == "??")
				it->second.function = getString(extractSymbolName(res[i++]));
		free(res);
	}
}

/*******************  FUNCTION  *********************/
char * SymbolResolver::extractSymbolName(char* value)
{
	//Vars
	char * ret = NULL;
	
	//errors
	assert(value != NULL);
	
	//search last
	char * pos1 = strrchr(value,'(');
	char * pos2 = strrchr(value,')');
	
	//scanf
	if (pos1 == NULL)
	{
		ret = value;
	} else if (pos2 == NULL) {
		ret = pos1;
	} else {
		*pos2 = '\0';
		ret = pos1+1;
	}
	
		//remove +0xXX
	int i = 0;
	while (ret[i] != '\0')
	{
		if (ret[i] == '+')
			ret[i] = '\0';
		else
			i++;
	}
	
	return ret;
}

/*******************  FUNCTION  *********************/
bool SymbolResolver::isSameFuntion(const CallSite* s1, void* s2) const
{
	if (s1 == NULL || s2 == NULL)
		return false;
	
	const CallSite * ss2 = getCallSiteInfo(s2);
	if (ss2 == NULL)
		return false;
	
	if (ss2 == s1)
		return true;
	
	if (s1->function <= 0 || ss2->function <= 0)
		return false;
	
	return (s1->mapEntry == ss2->mapEntry && s1->function == ss2->function);
}

}
