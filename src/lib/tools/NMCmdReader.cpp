/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
//standard
#include "NMCmdReader.hpp"
#include <cassert>
#include <sstream>
#include <cstdio>
#include <common/Debug.hpp>

/*******************  NAMESPACE  ********************/
namespace MATT
{

/*******************  FUNCTION  *********************/
NMCmdReader::NMCmdReader(void)
{

}

/*******************  FUNCTION  *********************/
NMCmdReader::~NMCmdReader(void)
{
	this->clear();
}

/*******************  FUNCTION  *********************/
void NMCmdReader::clear()
{
	this->binaryFile.clear();
	this->entries.clear();
}

/*******************  FUNCTION  *********************/
bool NMCmdReader::load(const std::string& binaryFile)
{
	//errors
	assert(binaryFile.empty() == false);
	
	//clear
	this->clear();

	//prepare cmds
	std::stringstream nmCmdNoDemangle;
	nmCmdNoDemangle << "nm --print-size -n --line-numbers -P --no-demangle " << binaryFile;
	std::stringstream nmCmdDemangle;
	nmCmdDemangle << "nm --print-size -n --line-numbers -P -C " << binaryFile;
	
	//start nm 1
	FILE * fpNoDem = popen(nmCmdNoDemangle.str().c_str(),"r");
	if (fpNoDem == NULL)
	{
		MATT_ERROR_ARG("Failed to use nm command as : %1 !").arg(nmCmdNoDemangle.str()).end();
		return false;
	}
	
	//start nm2
	FILE * fpDem = popen(nmCmdDemangle.str().c_str(),"r");
	if (fpDem == NULL)
	{
		MATT_ERROR_ARG("Failed to use nm command as : %1 !").arg(nmCmdDemangle.str()).end();
		fclose(fpNoDem);
		return false;
	}
	
	//vars
	NMCmdReaderEntry entryDemangle;
	NMCmdReaderEntry entryNoDemangle;
	while (!feof(fpDem) && !feof(fpNoDem))
	{
		//read first
		//int res1 = fscanf(fpNoDem,"%s %c %lx %lx %s:%d\n",);
		bool noDemangle = readNMLine(fpNoDem,entryNoDemangle);
		bool demangle = readNMLine(fpDem,entryDemangle);
		
		//skip unused for us
		if (noDemangle && entryNoDemangle.type != 'B' && entryNoDemangle.type != 'b')
			continue;
		
		assume(noDemangle == demangle,"Not same status on demangle and no demangle version of nm call.");
		
		//merge if ok
		if (demangle && noDemangle)
		{
			//check
			assumeArg(entryDemangle.offset == entryNoDemangle.offset,"No same offset on nm entries (%1 != %2) !").arg(entryDemangle.offset).arg(entryNoDemangle.offset).end();
			assumeArg(entryDemangle.size == entryNoDemangle.size,"No same size on nm entries (%1 != %2) !").arg(entryDemangle.size).arg(entryNoDemangle.size).end();
			//merge
			entryNoDemangle.degmangledName = entryDemangle.name;
			//insert
			entries.push_back(entryNoDemangle);
		}
	}

	//check
	assume(feof(fpDem) == feof(fpNoDem),"One nm instance finish faster than another, might be a bug !");
	
	//close
	fclose(fpDem);
	fclose(fpNoDem);
	
	//mark as done
	this->binaryFile = binaryFile;
	
	return true;
}

/*******************  FUNCTION  *********************/
void NMCmdReader::findSources(ElfGlobalVariableVector& vars) const
{
	//Errors
	assume(this->binaryFile.empty() == false,"Not binary files loaded, cannot resolve variable source sline declaration !");
	
	//search for all
	for (ElfGlobalVariableVector::iterator it = vars.begin() ; it != vars.end() ; ++it)
	{
		const NMCmdReaderEntry * entry = getEntry(it->name);
		if (entry != NULL)
		{
			fprintf(stderr,"NMEntry for %s : %s - %s - %s - %d\n",it->name.c_str(),entry->name.c_str(),entry->degmangledName.c_str(),entry->file.c_str(),entry->line);
			it->line = entry->line;
			it->file = entry->file;
		}
	}
}

/*******************  FUNCTION  *********************/
const NMCmdReaderEntry* NMCmdReader::getEntry(const std::string& name) const
{
	for (NMCmdReaderEntryVector::const_iterator it = entries.begin() ; it != entries.end() ; ++it)
	{
		if (it->name == name)
			return &(*it);
	}
	return NULL;
}

/*******************  FUNCTION  *********************/
bool NMCmdReader::readNMLine(FILE * fp,NMCmdReaderEntry& entry)
{
	char bufferName[4096];
	char bufferFile[2*4096];
	char buffer[8*4096];

	char * bres = fgets(buffer,sizeof(buffer),fp);
	if (bres == NULL)
		return false;

	int res = sscanf(bres,"%s %c %lx %lx %s\n",bufferName,&entry.type,&entry.offset,&entry.size,bufferFile);
	if (res == 5)
	{
		char * l = strrchr(bufferFile,':');
		if (l != NULL)
		{
			*l = '\0';
			l++;
			entry.line = atoi(l);
		} else {
			entry.line = -1;
		}
		//fprintf(stderr,"lines %s",bres);
		entry.name = bufferName;
		entry.file = bufferFile;
		return true;
	} else {
		//fprintf(stderr,"ignore lines %s",bres);
		return false;
	}
}

}
