#include <mpi.h>

namespace MATT
{
	struct Helpers
	{
		static int getFileId(void);
	};
	
	int Helpers::getFileId(void )
	{
		int rank;
		MPI_Comm_rank(MPI_COMM_WORLD, &rank);
		return rank;
	}
};
