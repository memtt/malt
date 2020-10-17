/*****************************************************
             PROJECT  : MALT
             VERSION  : 1.2.2
             DATE     : 10/2020
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MALT_NM_CMD_READER_HPP
#define MALT_NM_CMD_READER_HPP

/*******************  FUNCTION  *********************/
//standard
#include <string>
#include <vector>
#include "ELFReader.hpp"

/*******************  NAMESPACE  ********************/
namespace MALT
{

/********************  STRUCT  **********************/
struct NMCmdReaderEntry
{
	std::string name;
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
		void findSourcesAndDemangle(MALT::ElfGlobalVariableVector& vars) const;
		void clear();
	private:
		bool readNMLine(FILE * fp,NMCmdReaderEntry & entry);
		const NMCmdReaderEntry * getEntry(const std::string & name) const;
	private:
		std::string binaryFile;
		NMCmdReaderEntryVector entries;
};

}

#endif //MALT_ELF_READER_HPP
