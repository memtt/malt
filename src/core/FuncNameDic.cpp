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
#include "FuncNameDic.hpp"
#include <json/JsonState.h>
#include <common/Debug.hpp>

/*******************  NAMESPACE  ********************/
namespace MATT
{

/*******************  FUNCTION  *********************/
FuncNameDic::FuncNameDic(void )
{

}

/*******************  FUNCTION  *********************/
FuncNameDic::~FuncNameDic(void )
{
	for (FuncNameDicMap::const_iterator it = nameMap.begin() ; it != nameMap.end() ; ++it)
		free((void*)it->second);
}

/*******************  FUNCTION  *********************/
const char* FuncNameDic::getName(void* callSite)
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
void FuncNameDic::registerAddress(void* callSite)
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
const char* FuncNameDic::setupNewEntry(void* callSite)
{
	//errors
	assert(callSite != NULL);

	//use backtrace_symbol to extract name and line
	char ** tmp = backtrace_symbols(&callSite,1);
	//TODO move to assume
	assert(tmp != NULL);
	assert(tmp[0] != NULL);

	char * res = strdup(tmp[0]);
	nameMap[callSite] = res;
	free(tmp);

	return res;
}

/*******************  FUNCTION  *********************/
std::ostream& operator<<(std::ostream& out, const FuncNameDic& dic)
{
	for (FuncNameDicMap::const_iterator it = dic.nameMap.begin() ; it != dic.nameMap.end() ; ++it)
		out << it->first << " " << it->second << std::endl;
	return out;
}

/*******************  FUNCTION  *********************/
void convertToJson(htopml::JsonState& json, const FuncNameDic& value)
{
	json.openStruct();

// 	json.printField("entries",value.nameMap);
	
	json.printField("map",value.procMap);
	json.printField("strings",value.strings);
	json.printField("instr",value.callSiteMap);

	json.closeStruct();
}

/*******************  FUNCTION  *********************/
const char* FuncNameDic::setupNewEntry(void* callSite, const std::string& name)
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
void FuncNameDic::loadProcMap(void)
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
		fgets(buffer,sizeof(buffer),fp);
		
		//if ok, parse line
		if (!feof(fp))
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
	this->function = NULL;
	this->file = NULL;
}

/*******************  FUNCTION  *********************/
LinuxProcMapEntry* FuncNameDic::getMapEntry(void* callSite)
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
void FuncNameDic::resolveNames(void)
{
	//avoid to LD_PRELOAD otherwise we will create fork bomb
	setenv("LD_PRELOAD","",1);
	
	//loop on assemblies to extract names
	for (LinuxProcMap::iterator it = procMap.begin() ; it != procMap.end() ; ++it)
	{
		if (!(it->file.empty() || it->file[0] == '['))
			resolveNames(&(*it));
	}
}

/*******************  FUNCTION  *********************/
/*
 * Some links :
 * man proc & man addr2line
 * http://stackoverflow.com/a/7557756/257568
 * ​http://libglim.googlecode.com/svn/trunk/exception.hpp
 * ​http://stackoverflow.com/questions/10452847/backtrace-function-inside-shared-libraries 
*/
void FuncNameDic::resolveNames(LinuxProcMapEntry * procMapEntry)
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
				addr2lineCmd << ' '  << it->first;
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
	char bufferFunc[3*4096];
	char bufferFile[2*4096];
	int i = 0;
	while (!feof(fp))
	{
		//read the two lines
		fgets(bufferFunc,sizeof(bufferFunc),fp);
		fgets(bufferFile,sizeof(bufferFile),fp);
		
		if (feof(fp))
			break;
		
		//std::cerr << bufferFunc;
		//std::cerr << bufferFile;
		
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
std::string* FuncNameDic::getString(const char * value)
{
	for (std::vector<std::string>::iterator it = strings.begin() ; it != strings.end() ; ++it)
		if (*it == value)
			return &(*it);
	strings.push_back(value);
	return &(*strings.rbegin());
}

/*******************  FUNCTION  *********************/
void convertToJson(htopml::JsonState& json, const CallSite& value)
{
	if (value.mapEntry != NULL)
	{
		json.openStruct();
		if (value.file != NULL)// && *value.file != "??")
			json.printField("file",value.file);
		if (value.function != NULL)// && *value.function != "??")
			json.printField("function",value.function);
		if (value.line != 0)
			json.printField("line",value.line);
		json.printField("assembly",value.mapEntry);
		json.closeStruct();
	}
}

}
