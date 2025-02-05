/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.5
*    DATE     : 02/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/tools/NMCmdReader.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (ECR) - 2014
*    AUTHOR   : Sébastien Valat - 2024
***********************************************************/

#ifndef MALT_NM_CMD_READER_HPP
#define MALT_NM_CMD_READER_HPP

/**********************************************************/
//standard
#include <string>
#include <vector>
#include "ELFReader.hpp"

/**********************************************************/
namespace MALT
{

/**********************************************************/
struct NMCmdReaderEntry
{
	std::string name;
	char type;
	size_t offset;
	size_t size;
	std::string file;
	int line;
};

/**********************************************************/
typedef std::vector<NMCmdReaderEntry> NMCmdReaderEntryVector;

/**********************************************************/
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
