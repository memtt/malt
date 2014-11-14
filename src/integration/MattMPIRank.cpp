#include <mpi.h>
#include <sys/types.h>
#include <unistd.h>

namespace MALT
{

/********************* GLOBALS **********************/
static int mpiRank = -1;

/*******************  FUNCTION  *********************/
struct Helpers
{
	static int getFileId(void);
};

/*******************  FUNCTION  *********************/
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

/*******************  FUNCTION  *********************/
int MPI_Init(int *argc, char ***argv)
{
	//ensure MPI init
	int res = PMPI_Init(argc,argv);
	
	//extract rank
	MPI_Comm_rank(MPI_COMM_WORLD,&MALT::mpiRank);
	
	return res;
}

/*******************  FUNCTION  *********************/
int MPI_Init_thread(int *argc, char ***argv, int required, int *provided)
{
	//ensure MPI init
	int res = PMPI_Init_thread(argc,argv,required,provided);
	
	//extract rank
	MPI_Comm_rank(MPI_COMM_WORLD,&MALT::mpiRank);
	
	return res;
}

}
