/*****************************************************
             PROJECT  : numaprof
             VERSION  : 1.1.6
             DATE     : 06/2025
             AUTHOR   : Valat Sébastien - CERN
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef NUMAPROF_STATIC_ASSO_CACHE_HPP
#define NUMAPROF_STATIC_ASSO_CACHE_HPP

/********************  HEADERS  *********************/
//std
#include <cstdlib>
#include <config.h>

/********************  MACRO  ***********************/
#ifdef MALT_ENABLE_CODE_TIMING
	#define NUMAPROF_CACHE_STATS
#endif //MALT_ENABLE_CODE_TIMING

/*******************  NAMESPACE  ********************/
namespace numaprof
{

/*********************  CLASS  **********************/
/**
 * Implement a cache to store datas identified by addresses. The cache is built
 * as an associative cache with N ways and M rows per way. It is built in template
 * to be faster and optimized by compiler.
**/
template<class T,size_t ways,size_t rows>
class StaticAssoCache
{
	public:
		StaticAssoCache(void);
		void flush(void);
		const T * get(size_t addr) const;
		void set(size_t addr,const T & value);
		void unset(size_t addr);
		void printStats(const char * name) const;
	private:
		/** Store content **/
		T content[rows][ways];
		/** store addresses identifying content to match requests **/
		size_t addr[rows][ways];
		/** Used to round robin on rows to override **/
		unsigned char next[rows];
		/** count miss **/
		mutable size_t miss{0};
		/** counter hits **/
		mutable size_t hits{0};
		/** counter flush */
		mutable size_t flushCnt{0};
		/** Has been flushed recently, not do do again */
		bool dirty{true};
};

}

/**********************  IMPL  **********************/
#include "StaticAssoCache_impl.hpp"

#endif //NUMAPROF_STATIC_ASSO_CACHE_HPP
