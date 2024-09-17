/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.2
*    DATE     : 06/2023
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/tests/test-controler.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2016
***********************************************************/

/**********************************************************/
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <wrapper/malt.h>

/**********************************************************/
int main(void)
{
	//start disabling malt
	maltDisable();
	
	//ignored alloc
	void * buffer = malloc(1024);
	memset(buffer,0,1024);
	
	//enabling malt
	maltEnable();
	
	for (int i = 0 ; i < 100 ; i++)
	{
		void * buffer2 = malloc(1024);
		memset(buffer2,0,1024);
		free(buffer2);
	}
	
	//disabling malt
	maltDisable();
	
	//ignored free
	free(buffer);

	return 0;
}
