/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 02/2015
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
#include "AllocSizeDistribution.hpp"
#include <common/Helpers.hpp>
#include <json/JsonState.h>

/*******************  NAMESPACE  ********************/
namespace MATT
{

/*******************  FUNCTION  *********************/
void AllocSizeDistribution::add ( size_t size )
{
	AllocSizeDistributionMap::iterator it = map.find(size);
	if (it == map.end())
	{
		map[size] = 1;
	} else {
		it->second++;
	}
}

/*********************  TYPES  **********************/
void convertToJson ( htopml::JsonState& json, const AllocSizeDistribution& value )
{
	char buffer[64];
	json.openStruct();
		foreachConst(AllocSizeDistributionMap,value.map,it)
		{
			sprintf(buffer,"%lu",it->first);
			json.printField(buffer,it->second);
		}
	json.closeStruct();
}

}
