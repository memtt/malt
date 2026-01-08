/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 10/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/core/DomainCounters.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

#ifndef MALT_DOMAIN_COUNTERS_HPP
#define MALT_DOMAIN_COUNTERS_HPP

/**********************************************************/
//standard
#include <cassert>
//extern deps
#include <json/JsonState.h>
//malt
#include <common/Debug.hpp>

/**********************************************************/
namespace MALT
{

/**********************************************************/
/**
 * @brief Define the language domains supported by MALT.
 */
enum AllocDomain
{
	/** Domain C, accouting also C++, Rust, Fortran */
	DOMAIN_C_ALLOC,
	/** Domain for small objects in python. */
	DOMAIN_PYTHON_OBJ,
	/** Domain for medium memory segments in python. */
	DOMAIN_PYTHON_MEM,
	/** Domain for large memory segements in python. */
	DOMAIN_PYTHON_RAW,
	/** Domain for low level memory mapping. */
	DOMAIN_MMAP,
	/** Domain for GPU memory managment. */
	DOMAIN_GPU_ALLOC,
	/** Number of allocation domain. */
	ALLOC_DOMAIN_COUNT,
};

/**********************************************************/
/**
 * @brief Keep track of the number of operations performed per language domain.
 */
class DomainCounters
{
	public:
		DomainCounters(void);
		~DomainCounters(void);
		inline void countAlloc(AllocDomain domain, size_t mem);
		inline void countFree(AllocDomain domain, size_t mem);
		inline void updatePeak(AllocDomain domain);
	public:
		friend void convertToJson(htopml::JsonState& json, const DomainCounters & value);
	private:
		/** Count number of operations. */
		size_t counters[ALLOC_DOMAIN_COUNT];
		/** Amount total managed by domain. */
		size_t sum[ALLOC_DOMAIN_COUNT];
		/** Amount of memory per domain. */
		size_t mem[ALLOC_DOMAIN_COUNT];
		/** Memory peak of the domain */
		size_t localPeak[ALLOC_DOMAIN_COUNT];
		/** peak value */
		size_t atGlobalPeak[ALLOC_DOMAIN_COUNT];
};

/**********************************************************/
/**
 * Increase the counters about 1 call.
 * @param domain Language domain in which the call rise up.
 * @param mem The amount of memory requested.
 */
void DomainCounters::countAlloc(AllocDomain domain, size_t mem)
{
	//check
	assert(domain >=0 && domain < ALLOC_DOMAIN_COUNT);

	//increement
	this->counters[domain]++;
	this->sum[domain] += mem;
	this->mem[domain] += mem;
	
	//peak
	if (this->mem[domain] > this->localPeak[domain])
		this->localPeak[domain] = this->mem[domain];
}

/**********************************************************/
/**
 * Decrease the counters about peak memory about the size.
 * @param domain Language domain in which the call rise up.
 * @param mem The amount of memory freed.
 */
void DomainCounters::countFree(AllocDomain domain, size_t mem)
{
	//check
	assert(domain >=0 && domain < ALLOC_DOMAIN_COUNT);

	//decrement
	if (this->mem[domain] >= mem)
		this->mem[domain] -= mem;
}

/**********************************************************/
void DomainCounters::updatePeak(AllocDomain domain)
{
	//check
	assert(domain >=0 && domain < ALLOC_DOMAIN_COUNT);

	//copy
	this->atGlobalPeak[domain] = this->mem[domain];
}

}

#endif //MALT_DOMAIN_COUNTER_HPP
