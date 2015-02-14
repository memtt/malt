/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 02/2015
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
#include <tools/ELFReader.hpp>
#include <tools/NMCmdReader.hpp>
#include <common/Debug.hpp>
#include <portability/LinuxProcMapReader.hpp>
#include <cstdio>
#include "GlobalVariables.hpp"

/*******************  NAMESPACE  ********************/
namespace MATT
{
	
/*******************  FUNCTION  *********************/
void GlobalVariables::load ( void )
{
	//if not have libelf
	if (!ElfReader::hasLibElf())
		MATT_WARNING("MATT : warning, matt was compiled without libelf, you will not get global variable memory usage !\n");
	
	//load /proc/map
	LinuxProcMapReader map;
	map.load();
	
	//loop on files and load vars
	for (LinuxProcMapReader::const_iterator it = map.begin() ; it != map.end() ; ++it)
	{
		//if empty file
		if (it->file.empty())
			continue;
		
		//file not found (TODO move to portability and used better method, or manage error in ElfReader)
		FILE * fp = fopen(it->file.c_str(),"r");
		if (fp == NULL)
			continue;
		fclose(fp);
		
		//check exist
		if (globalVariables.find(it->file) == globalVariables.end())
		{
			//fprintf(stderr,"MATT: Need to load global variables from %s\n",it->file.c_str());
			
			//open elf
			ElfReader elfReader(it->file);

			//extract
			elfReader.loadGlobalVariables(globalVariables[it->file]);
			
			//search sources
			NMCmdReader reader;
			reader.load(it->file);
			reader.findSourcesAndDemangle(globalVariables[it->file]);
		}
	}
}

/*******************  FUNCTION  *********************/
void convertToJson ( htopml::JsonState& json, const MATT::GlobalVariables& value )
{
	convertToJson(json,value.globalVariables);
}
	
}
