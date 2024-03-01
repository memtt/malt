/*****************************************************
             PROJECT  : MALT
             VERSION  : 1.2.2
             DATE     : 06/2023
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
//c++
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <iostream>
//unix
#include <dlfcn.h>
#include <execinfo.h>
#include <link.h>
//externals
#include <json/JsonState.h>
//malt
#include <common/Debug.hpp>
#include <common/Array.hpp>
#include <common/Options.hpp>
#include <portability/OS.hpp>
//current
#include "SymbolSolver.hpp"

#ifdef __linux__
	#include <sys/user.h>
	#include <linux/binfmts.h>
	#ifndef PAGE_SIZE
		//@note: on some system MAX_ARG_STRLEN is defined to PAGE_SIZE
		//@todo: Find a better fix by removing completely the usage of
		//       MAX_ARG_STRLEN.
		#define PAGE_SIZE 4096
	#endif
#else
	#define MAX_ARG_STRLEN 131072
#endif


/*******************  NAMESPACE  ********************/
namespace MALT
{

/*******************  FUNCTION  *********************/
/**
 * Constructor of the symbol solver.
**/
SymbolSolver::SymbolSolver(void )
{
	strings.push_back("??");
}

/*******************  FUNCTION  *********************/
/**
 * Destructor of the symbol solver.
**/
SymbolSolver::~SymbolSolver(void )
{
	for (FuncNameDicMap::const_iterator it = nameMap.begin() ; it != nameMap.end() ; ++it)
		free((void*)it->second);
}

/*******************  FUNCTION  *********************/
/**
 * Get the symbol name from the call site raw address.
 * @param callsite Raw address of the call site.
 * @return The symbol name.
**/
const char* SymbolSolver::getName(void* callSite)
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
/**
 * Register a new raw address.
 * @param callsite Raw address of the callsite to convert with detailed informations.
**/
void SymbolSolver::registerAddress(void* callSite)
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
/**
 * Setup a new entry in the call site map with symbols name.
 * @param callsite Raw address of the call site we need to symbol name.
**/
const char* SymbolSolver::setupNewEntry(void* callSite)
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
/**
 * Dump the symbol solver into the output stream for debugging.
 * @param out Reference to the output stream to use.
 * @param dic Reference to the symbol solber to dump.
 * @return Reference to the output stream after dump.
**/
std::ostream& operator<<(std::ostream& out, const SymbolSolver& dic)
{
	for (FuncNameDicMap::const_iterator it = dic.nameMap.begin() ; it != dic.nameMap.end() ; ++it)
		out << it->first << " " << it->second << std::endl;
	return out;
}

/*******************  FUNCTION  *********************/
/**
 * Convert the symbol solver to json format.
 * @param json Reference to the json state to make the conversion.
 * @param value Reference to the symbol solver to convert.
**/
void convertToJson(htopml::JsonState& json, const SymbolSolver& value)
{
	json.openStruct();

// 	json.printField("entries",value.nameMap);

	json.printField("map",value.procMap);
	json.printField("strings",value.strings);
	json.printField("instr",value.callSiteMap);

	json.closeStruct();
}

/*******************  FUNCTION  *********************/
/**
 * Add a new entry for the given call site to the symbol name map.
 * @param callsite Raw address of the call site.
 * @param name Default symbol name to register if it is a new entry.
 * @return Symbol name as a C string.
**/
const char* SymbolSolver::setupNewEntry(void* callSite, const std::string& name)
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
/**
 * Check is the /proc/self/maps has been loaded of not.
 * @return True if loaded, false otherwise.
**/
bool SymbolSolver::procMapIsLoaded(void) const
{
	return (procMap.empty() == false);
}

/*******************  FUNCTION  *********************/
/**
 * Load the /proc/self/maps entry.
**/
void SymbolSolver::loadProcMap(void)
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
			int cnt = sscanf(buffer,"%p-%p %s %p %s %zu %s\n",&(entry.lower),&(entry.upper),ignored,&(entry.offset),ignored,&ignored2,fileName);
			//printf("%s => %p - %p\n",buffer,entry.lower,entry.upper);

			//check args
			if (cnt == 7)
				entry.file = fileName;
			else if (cnt == 6)
				entry.file.clear();
			else
				MALT_FATAL_ARG("Invalid readline of proc/map entry : %1.").arg(buffer).end();

			//ok push
			procMap.push_back(entry);
		}
	}

	//close
	fclose(fp);
}

/*******************  FUNCTION  *********************/
/**
 * If ASLR is enabled in addition to usage of fPIE or dynamic libraries there
 * is an offset to extract to get real offset to use with addr2line to extract
 * symboles. This offset is obtained by reading header of elf file.
 *
 * We currently do it using readelf because it does not required libelf which
 * does not always have headers installed on HPC systems.
 *
 * @todo Implement a fallback using directly libelf when available has we
 * already use it for optional features.
**/
size_t SymbolSolver::extractElfVaddr(const std::string & obj) const
{
	//vars
	size_t res = 0;

	//check
	assert(obj.empty() == false);

	//build command
	std::stringstream readelfcmd;
	readelfcmd << "LC_ALL='C' readelf -h " << obj;

	//debug
	if (gblOptions->outputVerbosity >= MALT_VERBOSITY_VERBOSE)
		fprintf(stderr, "MALT: %s\n", readelfcmd.str().c_str());
	else
		readelfcmd << " 2> /dev/null";

	//call and read content
	//@todo make a function to run and extract content as used many times
	setenv("LD_PRELOAD", "", 1);
	FILE * fp = popen(readelfcmd.str().c_str(),"r");
	assumeArg(fp != NULL,"Failed to read vaddr via readelf in %1 : %2.").arg(obj).argStrErrno().end();

	//read content
	char buffer[4096];
	while (res == 0 && !feof(fp)) {
		//get next line
		char * tmp = fgets(buffer, sizeof(buffer), fp);

		//parse
		if (tmp != NULL) {
			if (sscanf(buffer, "  Entry point address:               %zx", &res) == 1) {
				break;
			}
		}
	}

	//close
	fclose(fp);

	//warn
	if (res == 0)
		MALT_WARNING_ARG("Do not get ELF entry point (vaddr) to be used for ASLR + fPIE/fPIC correction. This might lead to issues to extract source location on %1.")
			.arg(obj)
			.end();

	//debug
	//if (gblOptions->outputVerbosity >= MALT_VERBOSITY_VERBOSE)
	//	fprintf(stderr, "MALT: vaddr is 0x%zx\n", res);

	//ok
	return res;
}

/*******************  FUNCTION  *********************/
/**
 * Test is ASLR is enabled to know how to resolve symbols.
**/
bool SymbolSolver::hasASLREnabled(void) const
{
	std::string tmp = OS::loadTextFile("/proc/sys/kernel/randomize_va_space");
	return ! (tmp.empty() || tmp == "0");
}

/*******************  FUNCTION  *********************/
/**
 * Convert the proc map entry to json format.
 * @param json Reference to the json state to make the conversion.
 * @param value Reference to the proc map entry object to convert.
**/
void convertToJson(htopml::JsonState& json, const LinuxProcMapEntry& value)
{
	json.openStruct();
	json.printField("lower",value.lower);
	json.printField("upper",value.upper);
	json.printField("offset",value.offset);
	json.printField("aslrOffset", value.aslrOffset);
	json.printField("file",value.file);
	json.closeStruct();
}

/*******************  FUNCTION  *********************/
/**
 * Constructor of the call site. It sets default line/function/file values to
 * unknown.
 * @param mapEntry Pointer to the related /proc/self/maps entry.
**/
CallSite::CallSite(LinuxProcMapEntry* mapEntry)
{
	this->mapEntry = mapEntry;
	this->line = 0;
	this->function = -1;
	this->file = -1;
}

/*******************  FUNCTION  *********************/
/**
 * Get the linux proc map entry from the call site raw address.
 * @param callsite Raw address of the call site.
 * @return Return a pointer to the struct representing the /proc/self/maps entry. NULL if not found.
**/
LinuxProcMapEntry* SymbolSolver::getMapEntry(void* callSite)
{
	//search in map by checking intervals
	for (LinuxProcMap::iterator it = procMap.begin() ; it != procMap.end() ; ++it)
		if (callSite >= it->lower && callSite < it->upper)
			return &(*it);

	//check errors
	if (callSite != (void*)0x1)
		MALT_WARNING_ARG("Caution, call site is not found in procMap : %1.").arg(callSite).end();
	return NULL;
}

/*******************  FUNCTION  *********************/
/**
 * Solve de symbols which have been registered by registerAddress().
 * Symbols are soved with the addr2line tool.
**/
void SymbolSolver::solveNames(void)
{
	//first try with maqao infos
	if (!maqaoSites.empty())
	{
		if (gblOptions != NULL && gblOptions->outputVerbosity >= MALT_VERBOSITY_DEFAULT)
			fprintf(stderr,"MALT : Resolving symbols with maqao infos...\n");
		this->solveMaqaoNames();
	}

	if (gblOptions != NULL && gblOptions->outputVerbosity >= MALT_VERBOSITY_DEFAULT)
		fprintf(stderr,"MALT : Resolving symbols with addr2line...\n");

	//avoid to LD_PRELOAD otherwise we will create fork bomb
	setenv("LD_PRELOAD","",1);

	//loop on assemblies to extract names
	for (LinuxProcMap::iterator it = procMap.begin() ; it != procMap.end() ; ++it)
	{
		if (!(it->file.empty() || it->file[0] == '['))
			solveNames(&(*it));
	}

	//final check for not found
	solveMissings();
}

/*******************  FUNCTION  *********************/
/**
 * Just search for the ASLR offset for each proc map entry used so we
 * can eventually solve again the symbole in post-mortem.
*/
void SymbolSolver::solveAslrOffsets(void)
{
	//loop on assemblies to extract names
	for (LinuxProcMap::iterator it = procMap.begin() ; it != procMap.end() ; ++it)
	{
		if (!(it->file.empty() || it->file[0] == '['))
		{
			//create addr2line args
			for (CallSiteMap::iterator it2 = callSiteMap.begin() ; it2 != callSiteMap.end() ; ++it2)
			{
				if (it2->second.mapEntry == &*it) {
					it->aslrOffset = this->getASRLOffset(it2->first);
					break;
				}
			}
		}
	}
}

/*******************  FUNCTION  *********************/
/**
 * Solve de symbols which have been registered by registerAddress().
 * Symbols are solved via maqao.
**/
void SymbolSolver::solveMaqaoNames(void)
{
	for (CallSiteMap::iterator it = callSiteMap.begin() ; it != callSiteMap.end() ; ++it)
	{
		MaqaoSiteMap::iterator site = maqaoSites.find(it->first);
		if (site != maqaoSites.end())
		{
			it->second.file = getString(site->second.file);
			it->second.function = getString(site->second.function);
			it->second.line = site->second.line;
			assert(it->second.mapEntry == NULL);
		}
	}
}

/*******************  FUNCTION  *********************/
/**
 * Determine the offset to remove from the effective addres to get the address
 * in the binary file. It might need some tricks when ASLR is enabled which
 * is obtained by getaddr().
*/
void * SymbolSolver::getASRLOffset(void * instrAddr) const
{
	//From https://stackoverflow.com/questions/55066749/how-to-find-load-relocation-for-a-pie-binary
	Dl_info info;
	void *extra = NULL;
	size_t elf2AddrOffset = 0;
	if (dladdr1(instrAddr, &info, &extra, RTLD_DL_LINKMAP)) {
		struct link_map *map = (struct link_map *)extra;
		elf2AddrOffset = map->l_addr;
	}
	return (void*)elf2AddrOffset;
}

/*******************  FUNCTION  *********************/
/* 
 * Some links :
 * man proc & man addr2line
 * http://stackoverflow.com/a/7557756/257568
 * ​http://libglim.googlecode.com/svn/trunk/exception.hpp
 * ​http://stackoverflow.com/questions/10452847/backtrace-function-inside-shared-libraries 
*/
/**
 * Solve the symbol names using addr2line for the given /proc/self/maps entry.
 * @param procMapEntry Pointer to the proc map entry to be solved.
**/
void SymbolSolver::solveNames(LinuxProcMapEntry * procMapEntry)
{
	//prepare command
	bool hasEntries = false;
	const std::string elfFile = procMapEntry->file;
	std::stringstream addr2lineCmd;
	const std::string prefix = std::string("addr2line -C -f -e ") + elfFile;
	const size_t prefixLen = prefix.length();
	addr2lineCmd << prefix;
	std::vector<CallSite*> lst;
	std::vector<std::string> theCommands;

	//create addr2line args
	bool firstNeedAslrScan = true;

	for (CallSiteMap::iterator it = callSiteMap.begin() ; it != callSiteMap.end() ; ++it)
	{
		if (it->second.mapEntry == procMapEntry)
		{
			if (addr2lineCmd.str().length() > MAX_ARG_STRLEN-20)
			{
				// Max length for each entry is 20 computed as follows:
				// In the worst case, each entry is 19 characters long
				// plus one for the newline in case it is the last entry.
				// We get 19 as: 1 for space, 2 for "0x", and 16 for address

				//hide error if silent
				if (gblOptions != NULL && gblOptions->outputVerbosity <= MALT_VERBOSITY_DEFAULT)
					addr2lineCmd << ' ' << "2>/dev/null";
				theCommands.push_back(addr2lineCmd.str());
				addr2lineCmd.str(std::string(""));
				addr2lineCmd << prefix;
			}
			if (firstNeedAslrScan)
			{
				procMapEntry->aslrOffset = this->getASRLOffset(it->first);
				firstNeedAslrScan = false;
			}

			//printf("OFFSET %zx %zx %zx %zx\n", it->first, procMapEntry->lower, map->l_addr, elfVaddr);
			addr2lineCmd << ' '  << (void*)(((size_t)it->first - (size_t)procMapEntry->aslrOffset));
			hasEntries = true;
			lst.push_back(&it->second);
		}
	}
	if ( addr2lineCmd.str().length() > prefixLen)
	{
		//hide error if silent
		if (gblOptions != NULL && gblOptions->outputVerbosity <= MALT_VERBOSITY_DEFAULT)
			addr2lineCmd << ' ' << "2>/dev/null";
		theCommands.push_back(addr2lineCmd.str());
	}

	//if no extry, exit
	if (!hasEntries)
		return;

	//run command
	//std::cerr << addr2lineCmd.str() << std::endl;
	size_t i = 0;
	for (auto &cmd : theCommands) {
		//debug
		if (gblOptions != NULL && gblOptions->outputVerbosity >= MALT_VERBOSITY_VERBOSE)
			printf("MALT: %s\n",cmd.c_str());

		FILE * fp = popen(cmd.c_str(),"r");

		//check error, skip resolution
		if (fp == NULL)
		{
			MALT_ERROR_ARG("Fail to use addr2line on %1 to load symbols : %2.").arg(elfFile).argStrErrno().end();
			return;
		}

		//read all entries (need big for some big template based C++ application,
		//seen at cern)
		static char bufferFunc[200*4096];
		static char bufferFile[20*4096];
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
			assumeArg(bufferFunc[endLine-1] == '\n',"Missing \\n at end of line for the function or symbol name read from addr2line : %1.").arg(bufferFunc).end();
			bufferFunc[endLine-1] = '\0';

			//check errors
			assume(i < lst.size(),"Overpass lst size.");

			//search ':' separator at end of "file:line" string
			char * sep = strrchr(bufferFile,':');
			if (sep == NULL)
			{
				MALT_WARNING_ARG("Fail to split source location on ':' : %1").arg(bufferFile).end();
			} else {
				*sep='\0';

				//extract line
				lst[i]->line = atoi(sep+1);

				//get filename and function name address
				lst[i]->file = getString(bufferFile);

				//if (strcmp(bufferFunc,"??") == 0)
				//      lst[i]->function = -1;
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
			MALT_ERROR_ARG("Get error while using addr2line on %1 to load symbols : %2.").arg(elfFile).argStrErrno().end();
			return;
		}
	}

	//error
	assumeArg(i == lst.size(),"Some entries are missing from addr2line, get %1, but expect %2. (%3)").arg(i).arg(lst.size()).arg(elfFile).end();
}

/*******************  FUNCTION  *********************/
/**
 * Return the string ID of the given value. If not already registered it register it
 * and assign an ID.
 * @param value The C string for which we want the string ID.
 * @return The string ID.
**/
int SymbolSolver::getString(const char * value)
{
	int id = 0;
	for (std::vector<std::string>::iterator it = strings.begin() ; it != strings.end() ; ++it)
	{
		if (*it == value)
			return id;
		id++;
	}
	if (*value == '\0')
		fprintf(stderr,"insert empty\n");
	strings.push_back(value);
	return strings.size()-1;
}

/*******************  FUNCTION  *********************/
/**
 * Register the maqao symbol.
 * @param funcId Function ID from maqao.
 * @param funcName The function name.
 * @param file The file of the call ste..
 * @param line Line of the call site.
**/
void SymbolSolver::registerMaqaoFunctionSymbol(int funcId, const char* funcName, const char* file, int line)
{
	MaqaoSite & site = maqaoSites[(void*)(size_t)funcId];
	site.function = funcName;
	puts(file);
	site.line = line;
	site.file = file;
}

/*******************  FUNCTION  *********************/
/**
 * Convert a call site to json format.
 * @param json Reference to the json state to make the conversion
 * @param value Reference to the call site to convert.
**/
void convertToJson(htopml::JsonState& json, const CallSite& value)
{
	json.openStruct();
	if (value.mapEntry != NULL || value.line != 0 || value.function != -1 || value.file != -1)
	{
		if (value.file != -1)// && *value.file != "??")
			json.printField("file",value.file);
		if (value.function != -1)// && *value.function != "??")
			json.printField("function",value.function);
		if (value.line != 0)
			json.printField("line",value.line);
		if (value.file != -1)
			json.printField("binary",value.file);
	}
	json.closeStruct();
}

/*******************  FUNCTION  *********************/
/**
 * Get the call site infos from the site raw address.
 * @param site The raw address of the symbol to solve.
 * @return Pointer to the call site.
**/
const CallSite* SymbolSolver::getCallSiteInfo(void* site) const
{
	CallSiteMap::const_iterator it = callSiteMap.find(site);
	if (it == callSiteMap.end())
		return NULL;
	else
		return &it->second;
}

/*******************  FUNCTION  *********************/
/**
 * Return the string from its ID.
 * @param id Define the ID of the string we want.
 * @return The string matching the given ID.
**/
const std::string& SymbolSolver::getString(int id) const
{
	assert((size_t)id < strings.size());
	return strings[id];
}

/*******************  FUNCTION  *********************/
/**
 * Function to use after loading symbols with resolveNames(). It search all '??' values
 * and try a resolution of symbol names with backtrace_symbols() from glibc.
**/
void SymbolSolver::solveMissings(void)
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
/**
 * Extract the symbol name from the backtrace_symbols().
 * @param value Value returned by backtrace_symbols()
 * @return The symbol name.
**/
char * SymbolSolver::extractSymbolName(char* value)
{
	//Vars
	char * ret = NULL;
	std::string old = value;

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

	if (*ret == 0)
	{
		ret = value;
		memcpy(value,old.c_str(),old.size());
	}

	return ret;
}

/*******************  FUNCTION  *********************/
/**
 * Check if it is the same function.
 * @param s1 Pointer to the call site.
 * @param s2 Raw address of the call site.
 * @return True if the resolved symbol match.
**/
bool SymbolSolver::isSameFuntion(const CallSite* s1, void* s2) const
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
