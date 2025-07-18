/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.3.1
*    DATE     : 06/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/core/DomainCounters.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
//standard
#include "DomainCounters.hpp"

/**********************************************************/
namespace MALT
{

/**********************************************************/
/**
 * @brief Constructor of the domain counter.
 * 
 * Init all the counters to 0.
 */
DomainCounters::DomainCounters(void)
{
	for (size_t i = 0 ; i < ALLOC_DOMAIN_COUNT ; i++) {
		this->counters[i] = 0;
		this->mem[i] = 0;
	}
}

/**********************************************************/
/**
 * @brief Destructor of the domain counters.
 */
DomainCounters::~DomainCounters(void)
{

}

/**********************************************************/
/**
 * @brief Convert the domain counters in JSON.
 * 
 * @param json The JSON stream to pipe in.
 * @param value The DomainCounter to dump.
 */
void convertToJson(htopml::JsonState& json, const DomainCounters & value)
{
	json.openStruct();
		json.openFieldStruct("counters");
			json.printField("c", value.counters[DOMAIN_C_ALLOC]);
			json.printField("pyObj", value.counters[DOMAIN_PYTHON_OBJ]);
			json.printField("pyMem", value.counters[DOMAIN_PYTHON_MEM]);
			json.printField("pyRaw", value.counters[DOMAIN_PYTHON_RAW]);
		json.closeFieldStruct("counters");
		json.openFieldStruct("mem");
			json.printField("c", value.mem[DOMAIN_C_ALLOC]);
			json.printField("pyObj", value.mem[DOMAIN_PYTHON_OBJ]);
			json.printField("pyMem", value.mem[DOMAIN_PYTHON_MEM]);
			json.printField("pyRaw", value.mem[DOMAIN_PYTHON_RAW]);
		json.closeFieldStruct("mem");
	json.closeStruct();
}

};
