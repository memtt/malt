/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.2
*    DATE     : 09/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/tools/ELFReader.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2014 - 2024
*    AUTHOR   : Sébastien Valat (ECR) - 2014
***********************************************************/

#ifndef MALT_ELF_READER_HPP
#define MALT_ELF_READER_HPP

/**********************************************************/
//standard
#include <vector>
#include <string>
#include <cstdio>
#include <json/JsonState.h>

/**********************************************************/
struct Elf;
struct Elf_Scn;

/**********************************************************/
namespace MALT
{

/**********************************************************/
/**
 * Definition of a global variable extracted from libelf.
**/
struct ElfGlobalVariable
{
	/** Name of the variable. **/
	std::string name;
	/** Size (in bytes) of the variable. **/
	size_t size;
	/** Offset of the variable inside elf file. **/
	size_t offset;
	/** Define if the variable is a TLS or not. **/
	bool tls;
	/** Source file defining the variable or empty if no debug informations. **/
	std::string file;
	/** Declaration line inside source file. **/
	int line;
};

/**********************************************************/
/** Struct to point entries of the ELF string table. **/
struct ElfStringTable
{
	/** Pointer to the C string. **/
	const char * data;
	/** Size of the C string. **/
	size_t size;
};

/**********************************************************/
/**
 * Define a vector container for global variables.
**/
typedef std::vector<ElfGlobalVariable> ElfGlobalVariableVector;

/**********************************************************/
/**
 * Wrapper of libelf to extract global variable list from binary file.
**/
class ElfReader
{
	public:
		ElfReader(const std::string & file);
		~ElfReader(void);
		void loadGlobalVariables(ElfGlobalVariableVector & variables);
		static bool hasLibElf(void);
	private:
		void libelfInit(void);
		void openFile(const std::string & file);
		void close(void);
		Elf_Scn* getSectionByType( size_t type );
		ElfStringTable getStringTable(int secId);
	private:
		/** Keep track of libelf handler until close. **/
		Elf * elf;
		/** Keep track of file descriptor used by libelf until close. **/
		FILE * fp;
};

/**********************************************************/
void convertToJson(htopml::JsonState& json, const ElfGlobalVariable& value);
// void convertToJson(htopml::JsonState& json, const ElfGlobalVariableVector & value);

}

#endif //MALT_ELF_READER_HPP
