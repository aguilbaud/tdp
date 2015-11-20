#include <mpi.h>
#include "mpi_fox.h"

int main(int argc, char *argv[])
{
  
  int comm_size;	
  int comm_rank;
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
  MPI_Comm_rank(MPI_COMM_WORLD, &comm_rank);	
  
  
  
  
  MPI_Finalize();

  return 0;
}
