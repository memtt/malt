/*****************************************************
             PROJECT  : MALT
             VERSION  : 1.2.2
             DATE     : 06/2023
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MALT_LINUX_PROC_MAP_HPP
#define MALT_LINUX_PROC_MAP_HPP

/********************  HEADERS  *********************/
//standard
#include <vector>
#include <string>

/*******************  NAMESPACE  ********************/
namespace MALT
{

/********************  STRUCT  **********************/
struct LinuxProcMapEntry
{
	void * lower;
	void * upper;
	void * offset;
	void * aslrOffset;
	std::string file;
};

/*********************  TYPES  **********************/
typedef std::vector<LinuxProcMapEntry> LinuxProcMap;

/*********************  CLASS  **********************/
class LinuxProcMapReader
{
	public:
		typedef LinuxProcMap::const_iterator const_iterator;
	public:
		void load(void);
		const LinuxProcMapEntry* getEntry(void* addr) const;
		const_iterator begin(void) { return procMap.begin(); };
		const_iterator end(void) { return procMap.end(); };
	private:
		LinuxProcMap procMap;
};

}

#endif //MALT_LINUX_PROC_MAP_HPP
