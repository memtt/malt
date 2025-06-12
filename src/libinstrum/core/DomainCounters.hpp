/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.6
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
enum AllocDomain
{
	DOMAIN_C_ALLOC,
	DOMAIN_PYTHON_OBJ,
	DOMAIN_PYTHON_MEM,
	DOMAIN_PYTHON_RAW,
	ALLOC_DOMAIN_COUNT,
};

/**********************************************************/
class DomainCounters
{
	public:
		DomainCounters(void);
		~DomainCounters(void);
		inline void countAlloc(AllocDomain domain, size_t mem);
	public:
		friend void convertToJson(htopml::JsonState& json, const DomainCounters & value);
	private:
		size_t counters[ALLOC_DOMAIN_COUNT];
		size_t mem[ALLOC_DOMAIN_COUNT];
};

/**********************************************************/
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
