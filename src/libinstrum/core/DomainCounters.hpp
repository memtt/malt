/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.3.1
*    DATE     : 06/2025
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
	public:
		friend void convertToJson(htopml::JsonState& json, const DomainCounters & value);
	private:
		/** Count number of operations. */
		size_t counters[ALLOC_DOMAIN_COUNT];
		/** Amount of memory per domain. */
		size_t mem[ALLOC_DOMAIN_COUNT];
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
	this->mem[domain] += mem;
}

}

#endif //MALT_DOMAIN_COUNTER_HPP
