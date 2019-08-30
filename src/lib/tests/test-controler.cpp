/*****************************************************
             PROJECT  : MALT
             VERSION  : 1.1.0
             DATE     : 08/2019
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <wrapper/malt.h>

/*******************  FUNCTION  *********************/
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
