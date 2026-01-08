/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 10/2025
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
		this->sum[i] = 0;
		this->localPeak[i] = 0;
		this->atGlobalPeak[i] = 0;
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
			json.printField("mmap", value.counters[DOMAIN_MMAP]);
			json.printField("gpu", value.counters[DOMAIN_GPU_ALLOC]);
		json.closeFieldStruct("counters");
		json.openFieldStruct("mem");
			json.printField("c", value.sum[DOMAIN_C_ALLOC]);
			json.printField("pyObj", value.sum[DOMAIN_PYTHON_OBJ]);
			json.printField("pyMem", value.sum[DOMAIN_PYTHON_MEM]);
			json.printField("pyRaw", value.sum[DOMAIN_PYTHON_RAW]);
			json.printField("mmap", value.sum[DOMAIN_MMAP]);
			json.printField("gpu", value.sum[DOMAIN_GPU_ALLOC]);
		json.closeFieldStruct("mem");
		json.openFieldStruct("localPeak");
			json.printField("c", value.localPeak[DOMAIN_C_ALLOC]);
			json.printField("pyObj", value.localPeak[DOMAIN_PYTHON_OBJ]);
			json.printField("pyMem", value.localPeak[DOMAIN_PYTHON_MEM]);
			json.printField("pyRaw", value.localPeak[DOMAIN_PYTHON_RAW]);
			json.printField("mmap", value.localPeak[DOMAIN_MMAP]);
			json.printField("gpu", value.localPeak[DOMAIN_GPU_ALLOC]);
		json.closeFieldStruct("localPeak");
		json.openFieldStruct("atGlobalPeak");
			json.printField("c", value.atGlobalPeak[DOMAIN_C_ALLOC]);
			json.printField("pyObj", value.atGlobalPeak[DOMAIN_PYTHON_OBJ]);
			json.printField("pyMem", value.atGlobalPeak[DOMAIN_PYTHON_MEM]);
			json.printField("pyRaw", value.atGlobalPeak[DOMAIN_PYTHON_RAW]);
			json.printField("mmap", value.atGlobalPeak[DOMAIN_MMAP]);
			json.printField("gpu", value.atGlobalPeak[DOMAIN_GPU_ALLOC]);
		json.closeFieldStruct("atGlobalPeak");
	json.closeStruct();
}

};
