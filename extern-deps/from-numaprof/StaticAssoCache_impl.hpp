/*****************************************************
             PROJECT  : numaprof
             VERSION  : 1.1.6
             DATE     : 06/2025
             AUTHOR   : Valat Sébastien - CERN
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef NUMAPROF_STATIC_ASSO_CACHE_IMPL_HPP
#define NUMAPROF_STATIC_ASSO_CACHE_IMPL_HPP

/********************  HEADERS  *********************/
//std
#include <cstdio>
#include <cassert>
#include "StaticAssoCache.hpp"

/********************  HEADERS  *********************/

/*******************  NAMESPACE  ********************/
namespace numaprof
{

/*******************  FUNCTION  *********************/
/**
 * Constructor of the cache, it reset all the content of the cache.
**/
template<class T,size_t ways,size_t rows>
StaticAssoCache<T,ways,rows>::StaticAssoCache(void)
{
	assert(ways < 256);
	flush();
}

/*******************  FUNCTION  *********************/
template<class T,size_t ways,size_t rows>
void StaticAssoCache<T,ways,rows>::flush(void)
{
	//tribial
	if (dirty == false)
		return;

	//count
	#ifdef NUMAPROF_CACHE_STATS
		this->flushCnt++;
	#endif //NUMAPROF_CACHE_STATS

	//flush
	for (int r = 0 ; r < rows ; r++)
	{
		next[r] = 0;
		for (int w = 0 ; w < ways ; w++)
			addr[r][w] = -1UL;
	}

	//remember done
	this->dirty = false;
}

/*******************  FUNCTION  *********************/
/**
 * Look in the cache and return the corresponding address.
**/
template<class T,size_t ways,size_t rows>
const T * StaticAssoCache<T,ways,rows>::get(size_t addr) const
{
	//compute row
	int r = addr % rows;

	//loop on ways
	for (int w = 0 ; w < ways ; w++)
	{
		if (this->addr[r][w] == addr)
		{
			//stats
			#ifdef NUMAPROF_CACHE_STATS
				hits++;
			#endif

			//ok
			return &content[r][w];
		}
	}
	
	//stats
	#ifdef NUMAPROF_CACHE_STATS
		miss++;
	#endif

	//not in cache
	return NULL;
}

/*******************  FUNCTION  *********************/
/**
 * Look in the cache and return the corresponding address.
**/
template<class T,size_t ways,size_t rows>
void StaticAssoCache<T,ways,rows>::set(size_t addr, const T & value)
{
	//compute row
	int r = addr % rows;
	
	//setup
	int w = next[r];
	this->content[r][w] = value;
	this->addr[r][w] = addr;
	
	//increment with round robin
	next[r] = (w+1)%ways;

	//mark dirty
	this->dirty = true;
}

/*******************  FUNCTION  *********************/
template<class T,size_t ways,size_t rows>
void StaticAssoCache<T,ways,rows>::unset(size_t addr)
{
	//compute row
	int r = addr % rows;

	//loop on ways
	for (int w = 0 ; w < ways ; w++)
	{
		if (this->addr[r][w] == addr)
			this->addr[r][w] = -1UL;
	}
}

/*******************  FUNCTION  *********************/
/**
 * Constructor of the cache, it reset all the content of the cache.
**/
template<class T,size_t ways,size_t rows>
void StaticAssoCache<T,ways,rows>::printStats(const char * name) const
{
	printf("%s cache hits: %zu, miss: %zu, ratio: %f, flush: %zu\n",name,hits,miss,(float)hits/((float)hits+(float)miss),flushCnt);
}

}

#endif //NUMAPROF_STATIC_ASSO_CACHE_IMPL_HPP
