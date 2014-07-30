/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MATT_ELF_READER_HPP
#define MATT_ELF_READER_HPP

/*******************  FUNCTION  *********************/
//standard
#include <vector>
#include <string>
#include <cstdio>
#include <json/JsonState.h>

/*********************  TYPES  **********************/
struct Elf;
struct Elf_Scn;

/*******************  NAMESPACE  ********************/
namespace MATT
{

/********************  STRUCT  **********************/
struct ElfGlobalVariable
{
	std::string name;
	size_t size;
	size_t offset;
	bool tls;
};

/********************  STRUCT  **********************/
struct ElfStringTable
{
	const char * data;
	size_t size;
};

/*********************  TYPES  **********************/
typedef std::vector<ElfGlobalVariable> ElfGlobalVariableVector;

/*********************  CLASS  **********************/
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
		Elf_Scn * getSectionByType(int type);
		ElfStringTable getStringTable(int secId);
	private:
		Elf * elf;
		FILE * fp;
};

/*******************  FUNCTION  *********************/
void convertToJson(htopml::JsonState& json, const ElfGlobalVariable& value);
// void convertToJson(htopml::JsonState& json, const ElfGlobalVariableVector & value);

}

#endif //MATT_ELF_READER_HPP
