/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 02/2015
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MATT_LINUX_PROC_MAP_HPP
#define MATT_LINUX_PROC_MAP_HPP

/********************  HEADERS  *********************/
//standard
#include <vector>
#include <string>

/*******************  NAMESPACE  ********************/
namespace MATT
{

/********************  STRUCT  **********************/
struct LinuxProcMapEntry
{
	void * lower;
	void * upper;
	void * offset;
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

#endif //MATT_LINUX_PROC_MAP_HPP
