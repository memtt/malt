/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MATT_NM_CMD_READER_HPP
#define MATT_NM_CMD_READER_HPP

/*******************  FUNCTION  *********************/
//standard
#include <string>
#include <vector>
#include "ELFReader.hpp"

/*******************  NAMESPACE  ********************/
namespace MATT
{

/********************  STRUCT  **********************/
struct NMCmdReaderEntry
{
	std::string name;
	std::string degmangledName;
	char type;
	size_t offset;
	size_t size;
	std::string file;
	int line;
};

/*********************  TYPES  **********************/
typedef std::vector<NMCmdReaderEntry> NMCmdReaderEntryVector;

/*********************  CLASS  **********************/
class NMCmdReader
{
	public:
		NMCmdReader(void);
		~NMCmdReader(void);
		bool load(const std::string & binaryFile);
		void findSources(MATT::ElfGlobalVariableVector& vars) const;
		void clear();
	private:
		bool readNMLine(FILE * fp,NMCmdReaderEntry & entry);
		const NMCmdReaderEntry * getEntry(const std::string & name) const;
	private:
		std::string binaryFile;
		NMCmdReaderEntryVector entries;
};

}

#endif //MATT_ELF_READER_HPP
