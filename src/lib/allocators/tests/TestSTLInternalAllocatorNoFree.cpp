/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 02/2015
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
//std
#include <map>
#include <list>
#include <vector>
//gtest
#include <gtest/gtest.h>
//internals
#include <allocators/STLInternalAllocatorNoFree.hpp>

/***************** USING NAMESPACE ******************/
using namespace MATT;

/*******************  FUNCTION  *********************/
TEST(TestSTLInternalAllocatorNoFreeNoFree,withVector)
{
	//declare
	std::vector<int,STLInternalAllocatorNoFree<int> > container;
	
	//fill
	for (int i = 0 ; i < 1000 ; i++)
		container.push_back(i);
	
	//loop over
	for (std::vector<int,STLInternalAllocatorNoFree<int> >::const_iterator it = container.begin() ; it != container.end() ; ++it);
	
	//copy
	std::vector<int,STLInternalAllocatorNoFree<int> > container2(container);
	
	//clear
	container.clear();
}

/*******************  FUNCTION  *********************/
TEST(TestSTLInternalAllocatorNoFreeNoFree,withList)
{
	//declare
	std::list<int,STLInternalAllocatorNoFree<int> > container;
	
	//fill
	for (int i = 0 ; i < 1000 ; i++)
		container.push_back(i);
	
	//loop over
	for (std::list<int,STLInternalAllocatorNoFree<int> >::const_iterator it = container.begin() ; it != container.end() ; ++it);
	
	//copy
	std::list<int,STLInternalAllocatorNoFree<int> > container2(container);
	
	//clear
	container.clear();
}

/*******************  FUNCTION  *********************/
TEST(TestSTLInternalAllocatorNoFreeNoFree,withMap)
{
	//declare
	std::map<int,int,std::less<int>,STLInternalAllocatorNoFree<std::pair<int,int> > > container;
	
	//fill
	for (int i = 0 ; i < 1000 ; i++)
		container[i] = i;
	
	//loop over
	for (std::map<int,int,std::less<int>,STLInternalAllocatorNoFree<std::pair<int,int> > >::const_iterator it = container.begin() ; it != container.end() ; ++it);
	
	//copy
	std::map<int,int,std::less<int>,STLInternalAllocatorNoFree<std::pair<int,int> > > container2(container);
	
	//clear
	container.clear();
}
