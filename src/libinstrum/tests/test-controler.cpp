/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 10/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/tests/test-controler.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2016
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <state/malt.h>

/**********************************************************/
int main(void)
{
	//start disabling malt
	malt_disable();
	
	//ignored alloc
	void * buffer = malloc(1024);
	memset(buffer,0,1024);
	
	//enabling malt
	malt_enable();
	
	for (int i = 0 ; i < 100 ; i++)
	{
		void * buffer2 = malloc(1024);
		memset(buffer2,0,1024);
		free(buffer2);
	}
	
	//disabling malt
	malt_disable();
	
	//ignored free
	free(buffer);

	return 0;
}
