/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.6
*    DATE     : 02/2025
*    LICENSE  : CeCILL-C
*    FILE     : examples/cpp/step-2-problem-fragmentation.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <list>

/**********************************************************/
void memory_doing_things(void)
{
	//vars
	const size_t max_size = 512;
	const size_t cnt = 10000;
	std::list<char*> ptrs;

	//variate size
	for (size_t size = 0 ; size < max_size ; size+=8)
	{
		//allocate all
		for (size_t i = 0 ; i < cnt ; i++)
		{
			char * ptr = new char[size];
			memset(ptr, 0, size);
			ptrs.push_back(ptr);
		}

		//deallocate half of them
		int i = 0;
		for (auto it = ptrs.begin() ; it != ptrs.end() ; ++it)
		{
			if (i++ % 2 == 0)
			{
				delete *it;
				auto it_to_remove = it;
				it++;
				ptrs.erase(it_to_remove);
			}
		}
	}
}

/**********************************************************/
int main(void)
{
	memory_doing_things();
	return EXIT_SUCCESS;
}
