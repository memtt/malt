/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 09/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/integration/MaltMPIRank.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2014
*    AUTHOR   : Sébastien Valat (ECR) - 2014
*    AUTHOR   : Sébastien Valat (INRIA) - 2023 - 2025
***********************************************************/

#include <mpi.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstdio>

namespace MALT
{

/**********************************************************/
static int mpiRank = -1;

/**********************************************************/
struct Helpers
{
	static int getFileId(void);
	static bool fileIdIsRank(void);
};

/**********************************************************/
bool Helpers::fileIdIsRank(void)
{
	return true;
}

/**********************************************************/
int Helpers::getFileId(void )
{
	if (mpiRank == -1)
		return getpid();
	else
		return mpiRank;
}

};

//capture MPI init
extern "C"
{

/**********************************************************/
int MPI_Init(int *argc, char ***argv)
{
	//ensure MPI init
	int res = PMPI_Init(argc,argv);
	
	//extract rank
	MPI_Comm_rank(MPI_COMM_WORLD,&MALT::mpiRank);

	//ok
	return res;
}

/**********************************************************/
int MPI_Init_thread(int *argc, char ***argv, int required, int *provided)
{
	//ensure MPI init
	int res = PMPI_Init_thread(argc,argv,required,provided);
	
	//extract rank
	MPI_Comm_rank(MPI_COMM_WORLD,&MALT::mpiRank);
	
	return res;
}

}
