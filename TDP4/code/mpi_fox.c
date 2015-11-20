#include <stdio.h>
#include <mpi.h>
#include "mpi_fox.h"
#include "util.h"

void fox_scatter(int n, MPI_Datatype *mpi_type_bloc){
  //Declaring new type
  MPI_Datatype mpi_type_bloc1;
  MPI_Type_vector(BLOCK_SIZE, BLOCK_SIZE, n, MPI_DOUBLE, &mpi_type_bloc1);
  MPI_Type_create_resized(mpi_type_bloc1, 0, BLOCK_SIZE*sizeof(double), mpi_type_bloc);
  MPI_Type_commit(mpi_type_bloc);
  MPI_Type_free(&mpi_type_bloc1);
  
  //
}

void fox_compute();
void fox_gather();

void fox(char *input_path){
  
  int comm_size, comm_rank;
  int n, grid_dims_size;
  MPI_Comm grid_comm, column_comm, line_comm;
  MPI_Datatype mpi_type_bloc;
  MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
  MPI_Comm_rank(MPI_COMM_WORLD, &comm_rank);	
  
  /*One process load the matrice and scatter it*/
  if(comm_rank == 0){
    FILE *fp = fopen(input_path, "r");
    load_file(fp, &n);
    fclose(fp); 
  }

  /*Grid & comms creation*/
  grid_dims_size = n/BLOCK_SIZE;
  int dims[] = {grid_dims_size, grid_dims_size};
  int periods[] = {1, 0};
  int rem_dims[] = {1, 0};
  int rem_dims2[] = {0, 1};
  MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, 1, &grid_comm);
  MPI_Cart_sub(grid_comm, rem_dims, &column_comm);
  MPI_Cart_sub(grid_comm, rem_dims2, &line_comm);

  /*Scatter the matrix*/
  fox_scatter(n, &mpi_type_bloc);
  

  
}
