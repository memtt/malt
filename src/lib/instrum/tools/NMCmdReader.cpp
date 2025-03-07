/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 10/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/tools/NMCmdReader.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (ECR) - 2014
*    AUTHOR   : Sébastien Valat - 2015 - 2024
***********************************************************/

/**********************************************************/
//standard
#include "NMCmdReader.hpp"
#include <cassert>
#include <sstream>
#include <cstdio>
#include <common/Debug.hpp>
#include <common/Options.hpp>
#include <portability/Compiler.hpp>

/**********************************************************/
namespace MALT
{

/**********************************************************/
NMCmdReader::NMCmdReader(void)
{

}

/**********************************************************/
NMCmdReader::~NMCmdReader(void)
{
	this->clear();
}

/**********************************************************/
void NMCmdReader::clear()
{
	this->binaryFile.clear();
	this->entries.clear();
}

/**********************************************************/
bool NMCmdReader::load(const std::string& binaryFile)
{
	//errors
	assert(binaryFile.empty() == false);
	
	//clear
	this->clear();

	//prepare cmds
	std::stringstream nmCmd;
	nmCmd << "nm --print-size -l -n -P --no-demangle " << binaryFile;
	
	//hide error if silent
	if (gblOptions != NULL && gblOptions->outputVerbosity <= MALT_VERBOSITY_DEFAULT)
		 nmCmd << ' ' << "2>/dev/null";
	
	//start nm 1
	FILE * fp = popen(nmCmd.str().c_str(),"r");
	if (fp == NULL)
	{
		MALT_ERROR_ARG("Failed to use nm command as : %1 !").arg(nmCmd.str()).end();
		return false;
	}
	
	//vars
	NMCmdReaderEntry entry;
	while (!feof(fp))
	{
		//read first
		//int res1 = fscanf(fp,"%s %c %zx %zx %s:%d\n",);
		bool status = readNMLine(fp, entry);
		
		//insert if ok
		if (status && (entry.type == 'B' || entry.type == 'b' || entry.type == 'D' || entry.type == 'd'))
			entries.push_back(entry);
	}

	//close
	fclose(fp);
	
	//mark as done
	this->binaryFile = binaryFile;
	
	return true;
}

/**********************************************************/
void NMCmdReader::findSourcesAndDemangle(ElfGlobalVariableVector& vars) const
{
	//Errors
	assume(this->binaryFile.empty() == false,"Not binary files loaded, cannot resolve variable source sline declaration !");
	
	//search for all
	for (ElfGlobalVariableVector::iterator it = vars.begin() ; it != vars.end() ; ++it)
	{
		const NMCmdReaderEntry * entry = getEntry(it->name);
		
		//search for sources
		if (entry != NULL)
		{
			it->line = entry->line;
			it->file = entry->file;
		}

		//get short name to cut on recent GCC (eg. _ZSt4cout@GLIBCXX_3.4)
		std::string shortName = it->name;
		int pos = shortName.find("@");
		if (pos != std::string::npos)
			shortName = shortName.substr(0, pos);
		
		//demangle namespace
		it->name = Compiler::demangleCppNames(shortName);
	}
}

/**********************************************************/
const NMCmdReaderEntry* NMCmdReader::getEntry(const std::string& name) const
{
	for (NMCmdReaderEntryVector::const_iterator it = entries.begin() ; it != entries.end() ; ++it)
	{
		if (it->name == name)
			return &(*it);
	}
	return NULL;
}

/**********************************************************/
bool NMCmdReader::readNMLine(FILE * fp, NMCmdReaderEntry& entry)
{
	char bufferName[4096];
	char bufferFile[2*4096];
	char buffer[8*4096];

	char * bres = fgets(buffer,sizeof(buffer),fp);
	if (bres == NULL)
		return false;

	int res = sscanf(bres, "%s %c %zx %zx\t%s\n", bufferName, &entry.type, &entry.offset, &entry.size, bufferFile);
	if (res == 5)
	{
		char * l = strrchr(bufferFile, ':');
		if (l != NULL)
		{
			*l = '\0';
			l++;
			entry.line = atoi(l);
		} else {
			entry.line = -1;
		}
		entry.name = bufferName;
		entry.file = bufferFile;
		return true;
	} else {
		return false;
	}
}

}
