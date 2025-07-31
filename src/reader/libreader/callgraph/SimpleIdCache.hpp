/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.3.1
*    DATE     : 04/2025
*    LICENSE  : CeCILL-C
*    FILE     : ./src/reader/libreader/callgraph/SimpleIdCache.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

#ifndef MALT_READER_SIMPLE_ID_CACHE_HPP
#define MALT_READER_SIMPLE_ID_CACHE_HPP

/**********************************************************/
#include <map>
#include <string>

/**********************************************************/
namespace MALTReader
{

/**********************************************************/
static const size_t CACHE_ID_NULL = -1;

/**********************************************************/
/**
 * SimpleIdCache class
 *
 * Tracks objects by assigning unique numeric ids to them.
 */
class SimpleIdCache
{
	public:
		SimpleIdCache(void);
		~SimpleIdCache(void);
		bool exists(const std::string & value) const;
		size_t put(const std::string & value);
		size_t get(const std::string & value) const;
	private:
		std::map<std::string, size_t> cache;
		size_t nextId{1};
};

}

#endif //MALT_READER_SIMPLE_ID_CACHE_HPP
