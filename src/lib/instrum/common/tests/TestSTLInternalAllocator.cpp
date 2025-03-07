/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 10/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/common/tests/TestSTLInternalAllocator.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2014 - 2024
*    AUTHOR   : Sébastien Valat (ECR) - 2014
***********************************************************/

/**********************************************************/
//std
#include <map>
#include <list>
#include <vector>
//gtest
#include <gtest/gtest.h>
//internals
#include <common/STLInternalAllocator.hpp>

/**********************************************************/
using namespace MALT;

/**********************************************************/
TEST(TestSTLInternalAllocator,withVector)
{
	//declare
	std::vector<int,STLInternalAllocator<int> > container;
	
	//fill
	for (int i = 0 ; i < 1000 ; i++)
		container.push_back(i);
	
	//loop over
	for (std::vector<int,STLInternalAllocator<int> >::const_iterator it = container.begin() ; it != container.end() ; ++it);
	
	//copy
	std::vector<int,STLInternalAllocator<int> > container2(container);
	
	//clear
	container.clear();
}

/**********************************************************/
TEST(TestSTLInternalAllocator,withList)
{
	//declare
	std::list<int,STLInternalAllocator<int> > container;
	
	//fill
	for (int i = 0 ; i < 1000 ; i++)
		container.push_back(i);
	
	//loop over
	for (std::list<int,STLInternalAllocator<int> >::const_iterator it = container.begin() ; it != container.end() ; ++it);
	
	//copy
	std::list<int,STLInternalAllocator<int> > container2(container);
	
	//clear
	container.clear();
}

/**********************************************************/
TEST(TestSTLInternalAllocator,withMap)
{
	//declare
	std::map<int,int,std::less<int>,STLInternalAllocator<std::pair<int,int> > > container;
	
	//fill
	for (int i = 0 ; i < 1000 ; i++)
		container[i] = i;
	
	//loop over
	for (std::map<int,int,std::less<int>,STLInternalAllocator<std::pair<int,int> > >::const_iterator it = container.begin() ; it != container.end() ; ++it);
	
	//copy
	std::map<int,int,std::less<int>,STLInternalAllocator<std::pair<int,int> > > container2(container);
	
	//clear
	container.clear();
}

/**********************************************************/
int main(int argc, char ** argv)
{
	//init internal allocator
	gblInternaAlloc = new SimpleAllocator(true);
	
	// This allows the user to override the flag on the command line.
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
