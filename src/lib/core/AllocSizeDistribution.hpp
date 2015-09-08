/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 02/2015
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MATT_ALLOC_SIZE_DISTRIBUTION_HPP
#define MATT_ALLOC_SIZE_DISTRIBUTION_HPP

/********************  HEADERS  *********************/
#include <map>
#include <cstdlib>
#include <allocators/STLInternalAllocatorNoFree.hpp>

/*********************  TYPES  **********************/
namespace htopml {
	class JsonState;
}

/*******************  NAMESPACE  ********************/
namespace MATT
{

/*********************  TYPES  **********************/
typedef std::map<size_t,size_t,std::less<size_t>,STLInternalAllocatorNoFree<std::pair<size_t,size_t> > > AllocSizeDistributionMap;

/*********************  CLASS  **********************/
class AllocSizeDistribution
{
	public:
		void add(size_t size);
		friend void convertToJson(htopml::JsonState & json,const AllocSizeDistribution & value);
	protected:
		AllocSizeDistributionMap map;
};

}

#endif //MATT_ALLOC_SIZE_DISTRIBUTION_HPP
