/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.3.0
*    DATE     : 06/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/portability/LinuxProcMapReader.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (ECR) - 2014
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

#ifndef MALT_LINUX_PROC_MAP_HPP
#define MALT_LINUX_PROC_MAP_HPP

/**********************************************************/
//standard
#include <vector>
#include <string>

/**********************************************************/
namespace MALT
{

/**********************************************************/
struct LinuxProcMapEntry
{
	void * lower{nullptr};
	void * upper{nullptr};
	void * offset{nullptr};
	size_t aslrOffset{-1UL};
	std::string file;
};

/**********************************************************/
typedef std::vector<LinuxProcMapEntry> LinuxProcMap;

/**********************************************************/
class LinuxProcMapReader
{
	public:
		typedef LinuxProcMap::const_iterator const_iterator;
	public:
		void load(void);
		const LinuxProcMapEntry* getEntry(void* addr) const;
		const LinuxProcMapEntry* getEntryByOffset(const std::string & fname, size_t offset) const;
		const LinuxProcMapEntry* getFirstEntry(const std::string & filename) const;
		const_iterator begin(void) { return procMap.begin(); };
		const_iterator end(void) { return procMap.end(); };
	private:
		LinuxProcMap procMap;
};

}

#endif //MALT_LINUX_PROC_MAP_HPP
