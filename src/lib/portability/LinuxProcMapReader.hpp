/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.2
*    DATE     : 06/2023
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/portability/LinuxProcMapReader.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (ECR) - 2014
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
	void * lower;
	void * upper;
	void * offset;
	void * aslrOffset;
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
		const_iterator begin(void) { return procMap.begin(); };
		const_iterator end(void) { return procMap.end(); };
	private:
		LinuxProcMap procMap;
};

}

#endif //MALT_LINUX_PROC_MAP_HPP
